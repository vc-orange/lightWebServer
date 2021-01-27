#ifndef LWS_TIMER_QUEUE
#define LWS_TIMER_QUEUE



#include <utility>
#include <set>
#include <vector>
#include "lwsChannel.h"
#include "lwsCallbacks.h"

class lws_event_loop;
class lws_timer;
class lws_timer_id;
class lws_time_stamp;




class lws_timer_queue{
public:
    lws_timer_queue(lws_event_loop* loop);
    ~lws_timer_queue();

    lws_timer_id add_timer(const timer_call_back& call_back,
                            lws_time_stamp when,
                            double interval);

private:

    typedef std::pair<lws_time_stamp,lws_timer*> entry;
    typedef std::set<entry> timer_list;

    void handle_read();

    std::vector<entry> get_expired(lws_time_stamp now);
    void reset(const std::vector<entry>& expired, lws_time_stamp now);

    bool insert(lws_timer* timer);

    lws_event_loop* __loop;
    const int __timer_fd;
    lws_channel __timer_fd_channel;

    timer_list __timers;
};


















#endif