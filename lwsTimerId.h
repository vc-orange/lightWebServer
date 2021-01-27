#ifndef LWS_TIMER_ID
#define LWS_TIMER_ID


class lws_timer;

class lws_timer_id{
public:
    explicit lws_timer_id(lws_timer* timer);
private:
    lws_timer* __value;
};

#endif