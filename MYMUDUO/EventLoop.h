#pragma once
#include <mutex>
#include <memory>
#include <vector>
#include <atomic>
#include <unistd.h>
#include<functional>
#include"Timestamp.h"
#include"noncopyable.h"
#include"CurrentThread.h"
class Channel;
class Poller;
//时间循环类，主要包含了两个大模块 Channel Poller (epoll的抽象)
class EventLoop : noncopyable
{
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    Timestamp pollReturnTime() const {return pollReturnTime_;}

    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);

    void wakeup();
    
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    bool isInLoopThread() const {return threadId_ == CurrentThread::tid();}
private:
    void handleRead();
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_;//原子操作，通过CAS实现的
    std::atomic_bool quit_;//标识退出loop循环
    std::atomic_bool callingPendingFunctors_;//标识当前loop是否有需要执行的回调操作
    const pid_t threadId_;//记录当前loop所在线程的id

    Timestamp pollReturnTime_;//poller返回发生时间的channel的时间点
    std::unique_ptr<Poller>poller_;

    int wakeupFd_;//主要作用，当mainLoop获取一个新用户的channel，通过轮询算法选择一个sutloop，通过该成员唤醒subloop处理channel

    std::unique_ptr<Channel>wakeupChannel_;
    ChannelList activeChannels_;
    Channel *currentActiveChannel_;

    std::vector<Functor>pendingFunctors_;//存储loop需要执行的所有的回调操作
    std::mutex mutex_;//互斥锁，用来保护上面vector容器的线程安全操作
};