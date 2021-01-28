#ifndef LWS_EVENT_LOOP_THREAD
#define LWS_EVENT_LOOP_THREAD


#include <mutex>
#include <condition_variable>
#include <thread>

class lws_event_loop;


class lws_event_loop_thread{
public:
    lws_event_loop_thread();
    ~lws_event_loop_thread();
    lws_event_loop* start_loop();
private:
    bool loop_exist();
    void thread_function();
    lws_event_loop* __loop;
    std::thread __thread_loop;
    std::mutex __mutex_cond;
    std::condition_variable __cond;
    bool __exit;
};


#endif