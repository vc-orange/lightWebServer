#include "lwsTimerQueue.h"
#include "lwsTimeStamp.h"
#include "lwsEventLoop.h"
#include "lwsTimer.h"
#include "lwsTimerId.h"
#include "lwsLog.h"
#include <string.h>
#include <assert.h>
#include <algorithm>
#include <sys/timerfd.h>

//返回期望的时间戳与当前时间戳的差值，并转换为标准时间
timespec time_from_now(lws_time_stamp expire)
{
    //时间戳差值
    int64_t micro_seconds = expire.micro_seconds_since_epoch() - lws_time_stamp::now().micro_seconds_since_epoch();

    //最低100毫秒
    if (micro_seconds < 100)
    {
        micro_seconds = 100;
    }
    timespec time_spec;
    time_spec.tv_sec = static_cast<time_t>(micro_seconds / lws_time_stamp::k_micro_second_per_second);
    time_spec.tv_nsec = static_cast<time_t>((micro_seconds % lws_time_stamp::k_micro_second_per_second) * 1000);
    return time_spec;
}

//创建timer_fd
int create_timer_fd()
{
    int timer_fd = timerfd_create(CLOCK_MONOTONIC,
                                  TFD_NONBLOCK | TFD_CLOEXEC);
    if (timer_fd < 0)
    {
        LOG_FATAL << "Failed in timerfd_create\n";
        abort();
    }
    return timer_fd;
}

//把对应的timerfd信号读掉
void read_timer_fd(int timer_fd, lws_time_stamp now)
{
    uint64_t size;
    ssize_t n = read(timer_fd, &size, sizeof(size));
    LOG_TRACE << "lws_timer_queue::handel_read() " << size
              << " at " << now.to_string() << '\n';
    if (n != sizeof(size))
    {
        LOG_ERROR << "lws_timer_queue::handel_read() reads "
                  << n << "bytes instead of 8 \n";
        abort();
    }
}

//重新设置timerfd，置入系统，等待调用
void reset_timer_fd(int timer_fd, lws_time_stamp expire)
{
    itimerspec newValue;
    itimerspec oldValue;
    memset(&newValue, 0, sizeof(newValue));
    memset(&oldValue, 0, sizeof(oldValue));
    newValue.it_value = time_from_now(expire);
    int ret = timerfd_settime(timer_fd, 0, &newValue, &oldValue);
    if (ret)
    {
        LOG_ERROR << "timerfd_settime() --timerfd_settime() \n";
        abort();
    }
}

//初始化时，先申请一个timerfd然后用channel监听这个fd的读事件，把回调函数handel_read交给它
lws_timer_queue::lws_timer_queue(lws_event_loop *loop)
    : __loop(loop),
      __timer_fd(create_timer_fd()),
      __timer_fd_channel(loop, __timer_fd),
      __timers()
{
    __timer_fd_channel.set_read_call_back(
        std::bind(&lws_timer_queue::handle_read, this));

    __timer_fd_channel.enable_reading();
}
//与eventloop共存亡，析构函数先不写
lws_timer_queue::~lws_timer_queue()
{
}

/*  处理读事件，首先得到当前的时间戳，然后把当前对应的fd信号读掉，让信号停止。
    然后得到所有已经过期的timer对象，调用这些timer对象的回调函数
    最后调用reset，重新插入repeat对象，delete一次性对象
*/
void lws_timer_queue::handle_read()
{
    __loop->assert_in_loop_thread();
    lws_time_stamp now(lws_time_stamp::now());
    read_timer_fd(__timer_fd, now);

    std::vector<entry> expired = get_expired(now);

    for (auto iter : expired)
    {
        iter.second->run();
    }

    reset(expired, now);
}

//从已经触发的timer中，找到需要循环的timer把它放回timer set中，不需要的timer直接delete
void lws_timer_queue::reset(const std::vector<entry> &expired, lws_time_stamp now)
{

    lws_time_stamp next_expire;
    for (auto &iter : expired)
    {
        if (iter.second->repeat())
        {
            //把上次的定时加上 循环间隔时间，重新插入timer队列
            iter.second->restart(now);
            insert(iter.second);
        }
        else
        {
            delete iter.second;
        }
    }

    if (!__timers.empty())
    {
        next_expire = __timers.begin()->second->expiration();
    }

    if (next_expire.valid())
    {
        reset_timer_fd(__timer_fd, next_expire);
    }
}

//找到并返回所有小于now的timer对象
std::vector<lws_timer_queue::entry> lws_timer_queue::get_expired(lws_time_stamp now)
{
    std::vector<entry> expired;
    //哨兵，哨兵的第一个元素为输入的时间戳NOW，另一个是最大的指针，所以使用LOWER_BOUND一定会找到时间大于NOW的ITER
    lws_timer_queue::entry sentry = {now, reinterpret_cast<lws_timer *>(UINTPTR_MAX)};
    auto iter = __timers.lower_bound(sentry);

    //如果没找到大于NOW的元素，意味着所有的元素都过期了，所以会返回END()
    //如果找到了，那么NOW一定是会小于这个ITER的TIME
    assert(iter == __timers.end() || now < iter->first);

    //把所有过期的元素拷贝到EXPIRED里面
    std::copy(__timers.begin(), iter, back_inserter(expired));
    //删除过期元素
    __timers.erase(__timers.begin(), iter);

    //返回过期列表
    return expired;
}

//从timer中取出期望时间，组成pair插入timer set中，等待下次调用
bool lws_timer_queue::insert(lws_timer *timer)
{
    bool earliest_changed = false;
    lws_time_stamp when = timer->expiration();
    auto iter = __timers.begin();
    if (iter == __timers.end() || when < iter->first)
    {
        earliest_changed = true;
    }
    auto ret = __timers.insert({when, timer});
    assert(ret.second);
    return earliest_changed;
}

lws_timer_id lws_timer_queue::add_timer(const timer_call_back &call_back,
                                        lws_time_stamp when,
                                        double interval)
{
    lws_timer *timer = new lws_timer(call_back, when, interval);
    __loop->run_in_loop(
        std::bind(&lws_timer_queue::add_timer_in_loop,
                  this,
                  timer));

    return lws_timer_id(timer);
}

void lws_timer_queue::add_timer_in_loop(lws_timer *timer)
{
    __loop->assert_in_loop_thread();
    bool earliest_changed = insert(timer);

    if (earliest_changed)
    {
        reset_timer_fd(__timer_fd, timer->expiration());
    }
}