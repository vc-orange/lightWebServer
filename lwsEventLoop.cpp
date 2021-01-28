#include "lwsEventLoop.h"
#include "lwsTimerQueue.h"
#include "lwsPoller.h"
#include "sys/eventfd.h"

const int k_poll_time_ms = 10000;

thread_local lws_event_loop *lws_event_loop::event_loop_singleton = nullptr;

int create_event_fd()
{
    int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (fd < 0)
    {
        LOG_ERROR << "lws_event_loop创建eventfd失败\n";
        abort();
    }
    return fd;
}

lws_event_loop *lws_event_loop::get_event_loop_singleton()
{
    return event_loop_singleton;
}

lws_event_loop::lws_event_loop()
    : __looping(false),
      __quit(false),
      __calling_pending_functors(false),
      __threadId(std::this_thread::get_id()),
      __poller(new lws_poller(this)),
      __timer_queue(new lws_timer_queue(this)),
      __wakeup_fd(create_event_fd()),
      __wakeup_channel(new lws_channel(this, __wakeup_fd))
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
    __wakeup_channel->set_read_call_back(
        std::bind(&lws_event_loop::handle_read, this));

    __wakeup_channel->enable_reading();
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
        do_pending_functors();
    }

    LOG_TRACE << "event loop " << this << " stop __looping \n";
    __looping = false;
}

void lws_event_loop::quit()
{
    __quit = true;
    if (!in_loop_thread())
    {
        wakeup();
    }
}

lws_timer_id lws_event_loop::run_at(const lws_time_stamp &time, const timer_call_back &call_back)
{
    return __timer_queue->add_timer(call_back, time, 0.0);
}

lws_timer_id lws_event_loop::run_after(double delay, const timer_call_back &call_back)
{
    lws_time_stamp time(lws_time_stamp::add_time(lws_time_stamp::now(), delay));
    return run_at(time, call_back);
}
lws_timer_id lws_event_loop::run_every(double interval, const timer_call_back &call_back)
{
    lws_time_stamp time(lws_time_stamp::add_time(lws_time_stamp::now(), interval));
    return __timer_queue->add_timer(call_back, time, interval);
}

//如果当前线程是I/O线程就直接执行
void lws_event_loop::run_in_loop(std::function<void()> call_back)
{
    if (in_loop_thread())
    {
        call_back();
    }
    else
    {
        queue_in_loop(call_back);
    }
}

//把CALL_BACK加入 任务队列，然后给唤醒fd一个信号，让LOOP有所反应
void lws_event_loop::queue_in_loop(std::function<void()> call_back)
{
    {
        std::lock_guard<std::mutex> lock(__mutex_pending_functors);
        __pending_functors.push_back(call_back);
    }

    if (!in_loop_thread() || __calling_pending_functors)
    {
        LOG_TRACE << "queue_in_loop 触发了 wakeup()\n";
        wakeup();
    }
}
//写入wakefd一个数字，触发系统信号
void lws_event_loop::wakeup()
{
    uint64_t signal = 1;
    ssize_t ret = eventfd_write(__wakeup_fd, signal);
    if (ret != 0)
    {
        LOG_ERROR << "lws_event_loop wakeup() 写入错误，返回值为 " << ret << "\n";
        abort();
    }
}

//把I/O线程限定任务队列换出来，然后依次执行
void lws_event_loop::do_pending_functors()
{
    std::vector<std::function<void()>> functors;
    __calling_pending_functors = true;
    {
        std::lock_guard<std::mutex> lock(__mutex_pending_functors);
        functors.swap(__pending_functors);
    }

    for (auto func : functors)
    {
        func();
    }
    __calling_pending_functors = false;
}
//从wakeup_fd读出信号
void lws_event_loop::handle_read()
{
    uint64_t signal = 1;
    ssize_t ret = eventfd_read(__wakeup_fd, &signal);
    if (ret != 0)
    {
        LOG_ERROR << "lws_event_loop wakeup() 读出错误，返回值为 " << ret << "\n";
        abort();
    }
}