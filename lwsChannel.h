#ifndef LWS_CHANNEL
#define LWS_CHANNEL
#include <functional>

class lws_event_loop;

class lws_channel
{
public:
    typedef std::function<void()> event_call_back;

    lws_channel(lws_event_loop *loop, int fd);

    void handle_event();
    void set_read_call_back(const event_call_back &call_back);
    void set_write_call_back(const event_call_back &call_back);
    void set_error_call_back(const event_call_back &call_back);

    int fd() const;
    int events() const;

    void set_revents(int rev);
    bool none_event() const;

    void enable_reading();

    int index();
    void set_index(int idx);

    lws_event_loop *owner_loop();

private:
    void update();

    static const int k_none_event;
    static const int k_read_event;
    static const int k_write_event;

    //持有该channel的eventloop
    lws_event_loop *__loop;

    //该channel监听的 fd，events，返回的revents，
    const int __fd;
    int __events;
    int __revents;
    //在活动channel队列中的下标，用于快速定位
    int __index;

    //三种回调事件
    event_call_back __read_call_back;
    event_call_back __write_call_back;
    event_call_back __error_call_back;
};

#endif