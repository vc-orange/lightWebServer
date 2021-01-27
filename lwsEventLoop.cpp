#include "lwsEventLoop.h"
#include "lwsTimerQueue.h"
#include "lwsPoller.h"
const int k_poll_time_ms = 10000;

thread_local lws_event_loop *lws_event_loop::event_loop_singleton = nullptr;

/*
lws_log & operator<<(lws_log& log,const lws_event_loop loop){
    return log<<loop.__threadId<<'\n';
}
*/

lws_event_loop *lws_event_loop::get_event_loop_singleton()
{
    return event_loop_singleton;
}

lws_event_loop::lws_event_loop()
    : __looping(false),
      __threadId(std::this_thread::get_id()),
      __poller(new lws_poller(this)),
      __timer_queue(new lws_timer_queue(this))
{
    LOG_TRACE << "event_loop created " << this << " in __threadId " << __threadId << '\n';
    if (event_loop_singleton)
    {
        LOG_FATAL << "Warning!!: "
                  << "another event_loop " << event_loop_singleton
                  << " has created in this thread: " << __threadId << '\n';
        abort();
    }
    else
    {
        event_loop_singleton = this;
    }
}

void lws_event_loop::update_channel(lws_channel *channel)
{
    assert(channel->owner_loop() == this);
    assert_in_loop_thread();
    __poller->update_channel(channel);
}
lws_event_loop::~lws_event_loop()
{
    assert(!__looping);
    event_loop_singleton = nullptr;
}

void lws_event_loop::assert_in_loop_thread()
{
    if (!in_loop_thread())
        abort_not_in_loop_thread();
}

bool lws_event_loop::in_loop_thread()
{
    return __threadId == std::this_thread::get_id();
}

void lws_event_loop::abort_not_in_loop_thread()
{
    LOG_FATAL << "this event pool not in loop thread \n";
    abort();
}

void lws_event_loop::loop()
{
    assert(!__looping);
    assert_in_loop_thread();
    __looping = true;
    __quit = false;

    while (!__quit)
    {
        __active_channels.clear();
        __poller->lws_poll(k_poll_time_ms, &__active_channels);
        for (auto &channel_ptr : __active_channels)
        {
            channel_ptr->handle_event();
        }
    }

    LOG_TRACE << "event loop " << this << " stop __looping \n";
    __looping = false;
}

void lws_event_loop::quit()
{
    __quit = true;
}

lws_timer_id lws_event_loop::run_at(const lws_time_stamp &time, const timer_call_back &call_back)
{
    return __timer_queue->add_timer(call_back, time, 0.0);
}

lws_timer_id lws_event_loop::run_after(double delay, const timer_call_back &call_back)
{
    lws_time_stamp time(lws_time_stamp::add_time(lws_time_stamp::now(), delay));
    return run_at(time,call_back);  
}
lws_timer_id lws_event_loop::run_every(double interval, const timer_call_back &call_back)
{
    lws_time_stamp time(lws_time_stamp::add_time(lws_time_stamp::now(), interval));
    return __timer_queue->add_timer(call_back, time, interval);
}