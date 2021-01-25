#ifndef LWS_EVENT_LOOP
#define LWS_EVENT_LOOP


#include <atomic>
#include <thread>
#include <cassert>
#include <unistd.h>
#include "lwsLog.h"
class lws_event_loop{
//单例模式
public:
static  lws_event_loop* get_event_loop_singleton();
private:
thread_local static lws_event_loop* event_loop_singleton;


public:
lws_event_loop();
~lws_event_loop();

void loop();
void assert_in_loop_thread();
bool in_loop_thread();
//friend lws_log& operator<<(lws_log& log,const lws_event_loop loop);
private:
//是否循环
std::atomic<bool> looping;
//当前线程ID
std::thread::id threadId;
void abort_not_in_loop_thread();

};





#endif