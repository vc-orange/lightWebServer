#ifndef LWS_POLLER
#define LWS_POLLER

#include <vector>
#include <map>
#include "lwsTimeStamp.h"
struct pollfd;
class lws_channel;
class lws_event_loop;

class lws_poller
{
public:
    typedef std::vector<lws_channel *> channel_list;

    lws_poller(lws_event_loop *loop);
    ~lws_poller();

    //poll I/O事件
    lws_time_stamp lws_poll(int timeoutMs, channel_list *active_channels);

    void update_channel(lws_channel *channel);

    void assert_in_loop_thread();

private:
    void fill_active_channels(int num_events,
                              channel_list *active_channels) const;
    lws_event_loop* __owner_loop;

    typedef std::vector<pollfd> pollfd_list;
    pollfd_list __pollfds;

    typedef std::map<int ,lws_channel* > channel_map;
    channel_map __channels;
};

#endif