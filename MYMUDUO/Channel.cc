#include"Channel.h"
#include "Logger.h"
#include<sys/epoll.h>
#include "EventLoop.h"

    const int Channel::kNoneEvent=0;
    const int Channel::kReadEvent=EPOLLIN | EPOLLPRI;
    const int Channel::kWriteEvent=EPOLLOUT;

    Channel::Channel(EventLoop *loop, int fd)
        : loop_(loop),fd_(fd),events_(0),revents_(0),index_(-1),tied_(false){}
    
    Channel::~Channel(){}

    void Channel::tie(const std::shared_ptr<void>& obj)
    {
        tie_ = obj;
        tied_ = true;

    }

    /*
        改变Channel所表示fd的events事件后，update负责在poller里面修改fd相应事件的epoll_ctl
        EventLoop => ChannelList Poller
    
    */

    void Channel::update()
    {
        //通过Channel所属的EventLoop，调用poller的相应方式，注册fd的events事件
        //add code  这一行主要是为了方便搜索
        loop_->updateChannel(this);
    }

    void Channel::remove()
    {
        //add code
        loop_->removeChannel(this);
    }

    //fd得到poller通知以后，处理事件
    void Channel::handleEvent(Timestamp receiveTime)
    {
        if(tied_)
        {
            std::shared_ptr<void> guard = tie_.lock();
            if(guard)
            {
                handleEventWithGuard(receiveTime);
            }
        }
        else
        {
            handleEventWithGuard(receiveTime);

        }
    }
    //根据poller通知，调用相应的回调函数
    void Channel::handleEventWithGuard(Timestamp receiveTime)
    {
        LOG_INFO("channel handleEvent revents:%d \n", revents_);

        if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
        {
            if(closeCallback_)
            {
                closeCallback_();
            }
        }

        if(revents_ & EPOLLERR)
        {
            if(errorCallback_)
            {
                errorCallback_();
            }
        }

        if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
        {
            if(readCallback_)
            {
                readCallback_(receiveTime);
            }
        }

        if(revents_ & EPOLLOUT)
        {
            if(writeCallback_)
            {
                writeCallback_();
            }
        }
    }