#include "lwsTimer.h"

lws_timer::lws_timer(const timer_call_back &call_back,
                     lws_time_stamp time,
                     double interval)
    : __call_back(call_back),
      __expiration(time),
      __interval(interval),
      __repeat(interval > 0.0)
{
}

void lws_timer::run() const
{
    __call_back();
}

lws_time_stamp lws_timer::expiration() const
{
    return __expiration;
}

bool lws_timer::repeat() const
{
    return __repeat;
}

void lws_timer::restart(lws_time_stamp now){
    if(__repeat){
        __expiration= lws_time_stamp::add_time(now,__interval);
    }else{
        __expiration= lws_time_stamp::invalid();
    }
}