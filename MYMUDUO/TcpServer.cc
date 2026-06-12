#include "TcpServer.h"

#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "Logger.h"
#include "TcpConnection.h"

#include <cstdio>
#include <functional>
#include <unistd.h>

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, 
    const std::string &nameArg, bool reuseport) : 
    loop_(loop), ipPort_(listenAddr.toIpPort()), name_(nameArg), 
    acceptor_(new Acceptor(loop, listenAddr, reuseport)), 
    threadPool_(new EventLoopThreadPool(loop, name_)), 
    connectionCallback_(defaultConnectionCallback), messageCallback_(defaultMessageCallback), 
    started_(0), nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(std::bind
        (&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    for(auto &item : connections_)
    {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::setThreadNum(int numThreads)
{
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
    if(started_++ == 0)
    {
        threadPool_->start();

        if(!acceptor_->listenning())
        {
            loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
        }
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    EventLoop *ioLoop = threadPool_->getNextLoop();

    char buf[64] = {0};
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);

    ++nextConnId_;

    std::string connName = name_ + buf;

    LOG_INFO("TcpServer::newConnection [%s] - new connection [%s] from %s\n", name_.c_str(), 
    connName.c_str(), peerAddr.toIpPort().c_str());

    InetAddress localAddr(0);

    TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));

    connections_[connName] = conn;

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s\n", name_.c_str(), conn->name().c_str());

    connections_.erase(conn->name());

    EventLoop *ioLoop = conn->getLoop();

    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}