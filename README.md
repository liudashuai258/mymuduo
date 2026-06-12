# MyMuduo

MyMuduo 是一个基于 C++11 和 Linux epoll 实现的简化版 muduo 网络库项目。

本项目参考 muduo 的核心设计思想，手写实现了 Reactor 网络模型中的主要模块，并完成了 EchoServer 回显服务器测试。

## 项目功能

- EventLoop 事件循环
- Channel 事件分发
- Poller / EPollPoller IO 多路复用
- Socket 封装
- Acceptor 新连接接收
- TcpConnection 连接管理
- Buffer 缓冲区
- TcpServer 服务器封装
- Thread / EventLoopThread / EventLoopThreadPool 线程模型
- EchoServer 回显服务器测试

## 项目结构

MYMUDUO/  
存放网络库核心源码。

muduo_server.cpp  
EchoServer 测试程序。

CMakeLists.txt  
项目构建文件。

## 核心流程

客户端连接服务器后，整体流程如下：

TcpServer  
-> Acceptor  
-> Socket::accept  
-> TcpConnection  
-> Channel  
-> EventLoop  
-> EPollPoller  
-> Buffer  
-> MessageCallback  
-> TcpConnection::send

## 编译方式

mkdir build  
cd build  
cmake ..  
make

## 运行方式

./muduo_server

另开一个终端测试：

nc 127.0.0.1 8000

输入：

hello

如果客户端返回 hello，说明 EchoServer 测试成功。

## 技术点

- C++11
- Linux epoll
- eventfd 跨线程唤醒
- Reactor 网络模型
- noncopyable 防拷贝
- std::function 回调机制
- shared_ptr / unique_ptr 管理对象生命周期
- Buffer 读写缓冲区
- TcpConnection 连接生命周期管理

## 当前状态

本项目已经完成基础网络库主线，实现了 TCP 连接建立、消息读取、消息回显和连接关闭等功能。
