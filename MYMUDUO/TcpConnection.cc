#include "TcpConnection.h"

#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Socket.h"

#include <cerrno>
#include <functional>
#include <unistd.h>

TcpConnection::TcpConnection(EventLoop *loop, const std::string &name,
    int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr) :
    loop_(loop), name_(name), state_(kConnecting), socket_(new Socket(sockfd)), 
    channel_(new Channel(loop, sockfd)), localAddr_(localAddr), peerAddr_(peerAddr)
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    LOG_INFO("TcpConnection::~TcpConnection [%s]\n", name_.c_str());
}

void TcpConnection::send(const std::string &message)
{
    if(state_ == kConnected)
    {
        if(loop_->isInLoopThread())
        {
            sendInLoop(message);
        }
        else
        {
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
        }
    }
}

void TcpConnection::sendInLoop(const std::string &message)
{
    ssize_t nwrote = 0;
    size_t remaining = message.size();

    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
    {
        nwrote = ::write(channel_->fd(), message.data(), message.size());

        if(nwrote >= 0)
        {
            remaining = message.size() - nwrote;

            if(remaining == 0 && writeCompleteCallback_)
            {
                loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
            }
        }
        else
        {
            nwrote = 0;

            if(errno != EWOULDBLOCK)
            {
                LOG_ERROR("TcpConnection::sendInLoop write error\n");
            }
        }
    }

    if(remaining > 0)
    {
        outputBuffer_.append(message.data() + nwrote, remaining);

        if(!channel_->isWriting())
        {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::shutdown()
{
    if(state_ == kConnected)
    {
        setState(kDisconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop()
{
    if(!channel_->isWriting())
    {
        socket_->shutdownWrite();
    }
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);

    if(n > 0)
    {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    }
    else if(n == 0)
    {
        handleClose();
    }
    else
    {
        errno = savedErrno;
        LOG_ERROR("TcpConnection::handleRead error\n");
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    if(channel_->isWriting())
    {
        ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());

        if(n > 0)
        {
            outputBuffer_.retrieve(n);

            if(outputBuffer_.readableBytes() == 0)
            {
                channel_->disableWriting();

                if(writeCompleteCallback_)
                {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                }

                if(state_ == kDisconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {
            LOG_ERROR("TcpConnection::handleWrite error\n");
        }
    }
}

void TcpConnection::handleClose()
{
    LOG_INFO("TcpConnection::handleClose fd=%d state=%d\n", channel_->fd(), state_);

    setState(kDisconnected);
    channel_->disableAll();

    TcpConnectionPtr guardThis(shared_from_this());

    if(connectionCallback_)
    {
        connectionCallback_(guardThis);
    }

    if(closeCallback_)
    {
        closeCallback_(guardThis);
    }
}

void TcpConnection::handleError()
{
    LOG_ERROR("TcpConnection::handleError name=%s\n", name_.c_str());
}

void TcpConnection::connectEstablished()
{
    setState(kConnected);

    channel_->tie(shared_from_this());
    channel_->enableReading();

    if(connectionCallback_)
    {
        connectionCallback_(shared_from_this());
    }
}

void TcpConnection::connectDestroyed()
{
    if(state_ == kConnected)
    {
        setState(kDisconnected);
        channel_->disableAll();

        if(connectionCallback_)
        {
            connectionCallback_(shared_from_this());
        }
    }

    channel_->remove();
}