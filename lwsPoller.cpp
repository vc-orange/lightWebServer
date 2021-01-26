#include "lwsPoller.h"

#include <assert.h>
#include <sys/poll.h>
#include "lwsChannel.h"
#include "lwsEventLoop.h"


lws_poller::lws_poller(lws_event_loop *loop)
    : __owner_loop(loop)
{
}

lws_poller::~lws_poller()
{
}

//待修改，暂不返回时间戳
void lws_poller::lws_poll(int timeoutMs, channel_list *active_channels)
{
    int num_events = poll(__pollfds.data(), __pollfds.size(), timeoutMs);
    //Timestamp now(Timestamp::now());
    if (num_events > 0)
    {
        LOG_TRACE << num_events << " 个事件发生\n";
        fill_active_channels(num_events, active_channels);
    }
    else if (num_events == 0)
    {
        LOG_TRACE << "没有事件\n";
    }
    else
    {
        LOG_ERROR << "lws_poll::lws_poll()\n";
    }
    //return now;
}

//接收一个fd，从channel map里找到对应fd的channel，挂到活动队列上
void lws_poller::fill_active_channels(int num_events,
                                      channel_list *active_channels) const
{
    for (const pollfd &pfd : __pollfds)
    {
        if (pfd.revents > 0)
        {
            num_events--;
            //从map里找到对应的channel，再检查该channel的fd属性是否正确
            auto channel_iter = __channels.find(pfd.fd);
            assert(channel_iter != __channels.end());
            lws_channel *channel = channel_iter->second;
            assert(channel->fd() == pfd.fd);

            channel->set_revents(pfd.revents);

            active_channels->push_back(channel);
        }
    }
}

void lws_poller::update_channel(lws_channel *channel)
{
    assert_in_loop_thread();
    LOG_TRACE << "fd= " << channel->fd()
              << " events= " << channel->events()<< '\n';
    if (channel->index() < 0)
    {
        //channel还未被加入pollfds数组
        assert(__channels.find(channel->fd()) == __channels.end());

        //建立对应的pfd
        pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        __pollfds.push_back(pfd);
        int idx = static_cast<int>(__pollfds.size()) - 1;
        channel->set_index(idx);
        __channels[pfd.fd] = channel;
    }
    else
    {
        //更新 已经存在的channel
        assert(__channels.find(channel->fd()) != __channels.end());
        assert(__channels[channel->fd()] == channel);
        int index = channel->index();
        assert(index >= 0 && index< static_cast<int> (__pollfds.size()));
        pollfd & pfd= __pollfds[index];
        
        assert(pfd.fd == channel->fd() || pfd.fd == -1);
        pfd.events  = static_cast<short> (channel->events());
        pfd.revents = 0;
        
        if(channel->none_event()){
            pfd.fd= -1;
        }
    }
}

void lws_poller::assert_in_loop_thread()
{
    __owner_loop->assert_in_loop_thread();
}