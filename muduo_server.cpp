#include "TcpServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Logger.h"
#include "Buffer.h"
#include "Callbacks.h"

#include <iostream>

void onConnection(const TcpConnectionPtr &conn)
{
    if(conn->connected())
    {
        std::cout << "新连接: " << conn->name() << std::endl;
    }
    else
    {
        std::cout << "连接断开: " << conn->name() << std::endl;
    }
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    std::string msg = buffer->retrieveAllAsString();

    std::cout << "收到消息: " << msg << std::endl;

    conn->send(msg);
}

int main()
{
    EventLoop loop;

    InetAddress listenAddr(8000);

    TcpServer server(&loop, listenAddr, "EchoServer", false);

    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    server.setThreadNum(0);

    server.start();

    loop.loop();

    return 0;
}