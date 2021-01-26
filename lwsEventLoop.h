#ifndef LWS_EVENT_LOOP
#define LWS_EVENT_LOOP


#include <atomic>
#include <thread>
#include <cassert>
#include <unistd.h>
#include <memory>
#include <vector>
#include "lwsLog.h"
#include "lwsChannel.h"



class lws_poller;

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
void quit();

void assert_in_loop_thread();
bool in_loop_thread();

void update_channel(lws_channel* chan);
//friend lws_log& operator<<(lws_log& log,const lws_event_loop loop);
private:
//是否循环
std::atomic<bool> __looping;
//是否退出
std::atomic<bool> __quit;
//当前线程ID
std::thread::id __threadId;
void abort_not_in_loop_thread();

typedef std::vector<lws_channel*> channel_list;
channel_list __active_channels;

std::unique_ptr<lws_poller> __poller;

};





#endif