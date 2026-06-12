#pragma once
#include <string>
#include "noncopyable.h"
using namespace std;

//LOG_INFO("%s %d",arg1,arg2)
#define LOG_INFO(logmsgFormat, ...) \
    do{\
        Logger &logger = Logger::instance();\
        logger.setLogLevel(INFO);\
        char buf[1024]={0};\
        snprintf(buf,1024,logmsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    }while(0)//要是不用do while(0)包裹，直接用{}包裹的话，如果用户写成if语句就会出问题

#define LOG_ERROR(logmsgFormat, ...) \
    do{\
        Logger &logger = Logger::instance();\
        logger.setLogLevel(ERROR);\
        char buf[1024]={0};\
        snprintf(buf,1024,logmsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    }while(0)

#define LOG_FATAL(logmsgFormat, ...) \
    do{\
        Logger &logger = Logger::instance();\
        logger.setLogLevel(FATAL);\
        char buf[1024]={0};\
        snprintf(buf,1024,logmsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    }while(0)
#ifdef MUDEBUG
#define LOG_DEBUG(logmsgFormat, ...) \
    do{\
        Logger &logger = Logger::instance();\
        logger.setLogLevel(DEBUG);\
        char buf[1024]={0};\
        snprintf(buf,1024,logmsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    }while(0)
#else
    #define LOG_DEBUG(logmsgFormat, ...)
#endif

//定义日志的级别 INFO(日志，正常信息) ERROR(错误，系统没事) FATAL(致命错误，系统崩溃) DEBUG(开发时的bug)
enum LogLevel
{
    INFO,//普通信息
    ERROR,//错误信息
    FATAL,//core错误
    DEBUG//调试信息
};

//输出一个日志
class Logger : noncopyable
{
public:
    //获取日志唯一的实例对象
    static Logger& instance();
    //设置日志级别
    void setLogLevel(int level);
    //写日志
    void log(string msg);
private:
    int loglevel_;
    Logger(){}
};