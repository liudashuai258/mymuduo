#pragma once

#include "Timestamp.h"

#include <functional>
#include <memory>

class Buffer;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;

using MessageCallback = std::function<void(const TcpConnectionPtr&,Buffer*,Timestamp)>;

inline void defaultConnectionCallback(const TcpConnectionPtr&)
{
}

inline void defaultMessageCallback(const TcpConnectionPtr&,Buffer*,Timestamp)
{
}