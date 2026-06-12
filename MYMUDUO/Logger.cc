#include "Logger.h"
#include "Timestamp.h"
#include <iostream>

using namespace std;
 //获取日志唯一的实例对象
Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}
//设置日志级别
void Logger::setLogLevel(int level)
{
    loglevel_=level;
}
//写日志
void Logger::log(string msg)
{
    switch (loglevel_)
    {    case INFO:
        cout<<"[INFO] ";
        break;
    case ERROR:
        cout<<"[ERROR] ";
        break;
    case FATAL:
        cout<<"[FATAL] ";
        break;
    case DEBUG:
        cout<<"[DEBUG] ";
        break;
    default:
        cout<<"[INFO] ";
        break;
    }

    //打印时间和msg
    cout<<Timestamp::now().toString()<<": "<<msg<<endl;
}