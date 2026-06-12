#pragma once

#include "Socket.h"
#include "Channel.h"
#include "noncopyable.h"


#include <functional>
class EventLoop;
class InetAddress;


class Acceptor : noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int Sockfd,const InetAddress&)>;
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb)
    {
        NewConnectionCallback_ = cb;
    }

    bool listenning() const {return listenning_;}
    void listen();
private:
    void handleRead();
    EventLoop *loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback NewConnectionCallback_;
    bool listenning_;
};