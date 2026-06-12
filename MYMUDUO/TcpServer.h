#pragma once

#include "noncopyable.h"
#include "Callbacks.h"
#include "InetAddress.h"

#include <atomic>
#include <map>
#include <memory>
#include <string>

class EventLoop;
class Acceptor;
class EventLoopThreadPool;

class TcpServer : noncopyable
{
public:
    TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg, bool reuseport = false);
    ~TcpServer();

    void setThreadNum(int numThreads);
    void start();

    void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

private:
    void newConnection(int sockfd, const InetAddress &peerAddr);
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

private:
    using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

    EventLoop *loop_;
    const std::string ipPort_;
    const std::string name_;

    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<EventLoopThreadPool> threadPool_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    std::atomic_int started_;
    int nextConnId_;

    ConnectionMap connections_;
};