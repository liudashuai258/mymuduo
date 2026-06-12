#pragma once

#include <memory>
#include<functional>
#include "Timestamp.h"
#include "noncopyable.h"

class EventLoop;

/*

Channel 理解为通道，封装了sockfd和其感兴趣的event,如EPOLLIN、EPOLLOUT事件
还绑定了poller返回的具体事件

*/

class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop* loop,int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTime);

    //设置回调函数对象
    void setReadCallback(ReadEventCallback cd) {readCallback_=std::move(cd);}
    void setWriteCallback(EventCallback cd) {writeCallback_ = std::move(cd);}
    void setCloseCallback(EventCallback cd) {closeCallback_ = std::move(cd);}
    void setErrorCallback(EventCallback cd) {errorCallback_ = std::move(cd);}
    //防止当chhannel被手动remove掉，channel还在执行回调操作
/*
*检查对象是否仍然存在（lock 返回 nullptr 表示已经销毁）。
*在作用域内临时延长对象生命周期，保证回调安全执行。
*避免 Channel 回调时访问悬空对象，解决典型的事件驱动程序生命周期问题。
*/
    void tie(const std::shared_ptr<void>&);

    int fd() const {return fd_;}
    int events() const {return events_;}
    void set_revents(int revt) { revents_ = revt; }
    //设置fd相应的事件状态
    void enableReading() {events_ |= kReadEvent; update();}
    void disableReading() {events_ &= ~kReadEvent; update();}
    void enableWriting() {events_ |= kWriteEvent; update();}
    void disableWriting() {events_ &= ~kWriteEvent; update();}
    void disableAll() {events_=kNoneEvent; update();}

    //返回fd当前的事件状态
    bool isNoneEvent() const {return events_==kNoneEvent;}
    bool isWriting() const {return events_ & kWriteEvent;}
    bool isReading() const {return events_ & kReadEvent;}

    int index() {return index_;}
    void set_index(int idx){index_=idx;}

    //one loop per thread
    EventLoop *ownerLoop(){return loop_;}
    void remove();
private:    

    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    //因为Channel通道里面能够获知fd最终发生的具体事件revents,所以它负责调用具体事件的回调操作
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};