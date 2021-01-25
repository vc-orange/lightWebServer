#include "lwsEventLoop.h"

thread_local lws_event_loop *lws_event_loop::event_loop_singleton = nullptr;

/*
lws_log & operator<<(lws_log& log,const lws_event_loop loop){
    return log<<loop.threadId<<'\n';
}
*/

lws_event_loop *lws_event_loop::get_event_loop_singleton()
{
    return event_loop_singleton;
}

lws_event_loop::lws_event_loop()
    : looping(false),
      threadId(std::this_thread::get_id())
{
    LOG_TRACE << "event_loop created " << this << "in threadId " << threadId << '\n';
    if (event_loop_singleton)
    {
        LOG_FATAL << "Warning!!: "
                  << "another event_loop " << event_loop_singleton
                  << " has created in this thread: " << threadId << '\n';
        abort();
    }
    else
    {
        event_loop_singleton = this;
    }
}

lws_event_loop::~lws_event_loop()
{
    assert(!looping);
    event_loop_singleton = nullptr;
}

void lws_event_loop::assert_in_loop_thread()
{
    if (!in_loop_thread())
        abort_not_in_loop_thread();
}

bool lws_event_loop::in_loop_thread()
{
    return threadId == std::this_thread::get_id();
}

void lws_event_loop::abort_not_in_loop_thread()
{
    LOG_FATAL << "this event pool not in loop thread \n";
    abort();
}

void lws_event_loop::loop()
{
    assert(!looping);
    assert_in_loop_thread();
    looping = true;

    sleep(5);

    LOG_TRACE << "event loop " << this << " stop looping \n";
    looping = false;
}