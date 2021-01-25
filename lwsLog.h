#define LOG_NOT_READY 1



#ifndef LWS_LOG
#define LWS_LOG

#ifdef LOG_NOT_READY
#include<iostream>
#define LOG_TRACE std::cout
#define LOG_DEBUG std::cout
#define LOG_INFO std::cout
#define LOG_WARN std::cout
#define LOG_ERROR std::cout
#define LOG_FATAL std::cout
#else
#define LOG_TRACE lws_log::lws_log_stream
#define LOG_DEBUG lws_log::lws_log_stream
#define LOG_INFO lws_log::lws_log_stream
#define LOG_WARN lws_log::lws_log_stream
#define LOG_ERROR lws_log::lws_log_stream
#define LOG_FATAL lws_log::lws_log_stream

#define lws_log::lws_log_stream std::cout
/*
class lws_log{
public:
    static lws_log lws_log_stream;

    template<class T>
    lws_log& operator<<(T& data);
};*/
#endif

#endif