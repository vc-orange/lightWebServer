#include "lwsChannel.h"
#include "lwsEventLoop.h"
#include <sys/poll.h>

const int lws_channel::k_none_event = 0;
const int lws_channel::k_read_event = POLLIN | POLLPRI;
const int lws_channel::k_write_event = POLLOUT;

lws_channel::lws_channel(lws_event_loop *loop, int fdArg)
    : __loop(loop),
      __fd(fdArg),
      __events(0),
      __revents(0),
      __index(-1)
{
}

void lws_channel::update()
{
    __loop->update_channel(this);
}

void lws_channel::handle_event()
{
    if(__revents & POLLNVAL){
        LOG_WARN<< "WARN :Channel::handle event() POLLNVAL\n ";
    }
    if(__revents & (POLLERR | POLLNVAL)){
        if(__error_call_back)
            __error_call_back();
    }
    if(__revents& (POLLIN|POLLPRI|POLLRDHUP)){
        if(__read_call_back)
            __read_call_back();
    }
    if(__revents& POLLOUT){
        if(__write_call_back)
            __write_call_back();
    }
}

void lws_channel::set_read_call_back(const event_call_back &call_back)
{
    __read_call_back = call_back;
}
void lws_channel::set_write_call_back(const event_call_back &call_back)
{
    __write_call_back = call_back;
}
void lws_channel::set_error_call_back(const event_call_back &call_back)
{
    __error_call_back = call_back;
}

int lws_channel::fd() const
{
    return __fd;
}
int lws_channel::events() const
{
    return __events;
}
void lws_channel::set_revents(int revents)
{
    __revents = revents;
}
bool lws_channel::none_event() const
{
    return __events == k_none_event;
}

void lws_channel::enable_reading()
{
    __events |= k_read_event;
    update();
}

int lws_channel::index()
{
    return __index;
}
void lws_channel::set_index(int idx)
{
    __index = idx;
}

lws_event_loop *lws_channel::owner_loop()
{
    return __loop;
}