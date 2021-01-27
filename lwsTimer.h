#ifndef LWS_TIMER
#define LWS_TIMER

#include "lwsCallbacks.h"
#include "lwsTimeStamp.h"
class lws_timer
{
public:
    lws_timer(const timer_call_back &call_back,
              lws_time_stamp time,
              double interval);

    void run()const;
    lws_time_stamp expiration() const;
    bool repeat() const ;
    void restart(lws_time_stamp now);
private:
    const timer_call_back __call_back;
    lws_time_stamp __expiration;
    const double __interval;
    const bool __repeat;
};

#endif