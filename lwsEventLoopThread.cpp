#include "lwsEventLoopThread.h"
#include "lwsEventLoop.h"
#include <assert.h>
#include <functional>
lws_event_loop_thread::lws_event_loop_thread()
    :__loop(nullptr),
    __exit(false)
{
}

lws_event_loop_thread::~lws_event_loop_thread()
{
    __exit=true;
    __loop->quit();
    __thread_loop.join();
}

bool lws_event_loop_thread::loop_exist(){
    return __loop!=nullptr;
}

lws_event_loop *lws_event_loop_thread::start_loop()
{
    assert(!__thread_loop.joinable());
    __thread_loop = std::thread(std::bind(&lws_event_loop_thread::thread_function,this));
    {
        std::unique_lock<std::mutex> lock(__mutex_cond);
        __cond.wait(lock,std::bind(&lws_event_loop_thread::loop_exist,this));
    }
    return __loop;
}

void lws_event_loop_thread::thread_function(){
    lws_event_loop loop;
    {
        std::lock_guard<std::mutex> lock(__mutex_cond);
        __loop = &loop;
        __cond.notify_one();
    }
    loop.loop();

}