#include "lwsTimerId.h"

lws_timer_id::lws_timer_id(lws_timer* timer)
    :__value(timer)
{
}