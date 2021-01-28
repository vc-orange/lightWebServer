#ifndef LWS_EVENT_LOOP
#define LWS_EVENT_LOOP


#include <atomic>
#include <thread>
#include <cassert>
#include <unistd.h>
#include <memory>
#include <vector>
#include <mutex>
#include "lwsLog.h"
#include "lwsChannel.h"
#include "lwsTimeStamp.h"
#include "lwsTimerId.h"
#include "lwsCallbacks.h"

class lws_poller;
class lws_timer_queue;
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

    lws_timer_id run_at(const lws_time_stamp& time,const timer_call_back& call_back);
    lws_timer_id run_after(double delay,const timer_call_back& call_back);
    lws_timer_id run_every(double interval,const timer_call_back& call_back);


    void run_in_loop(const std::function<void()> call_back);
    void queue_in_loop(const std::function<void()> call_back);
    
    void assert_in_loop_thread();
    bool in_loop_thread();

    void update_channel(lws_channel* chan);
    //friend lws_log& operator<<(lws_log& log,const lws_event_loop loop);
private:
    void wakeup();

    void handle_read();
    
    void do_pending_functors();
    //当前是否在执行I/O限定任务
    std::atomic<bool> __calling_pending_functors;
    //wake_up信号的 eventfd
    int __wakeup_fd;
    //监听wakeup_fd的channel
    std::unique_ptr<lws_channel> __wakeup_channel;
    //I/O限定任务队列的锁
    std::mutex __mutex_pending_functors;
    //I/O限定任务队列
    std::vector<std::function<void()>> __pending_functors;
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

    lws_timer_queue* __timer_queue;

};





#endif