#ifndef LWS_TIME_STAMP
#define LWS_TIME_STAMP

#include <cstdint>
#include <string>
class lws_time_stamp
{
public:
    lws_time_stamp();

    explicit lws_time_stamp(int64_t micro_seconds_since_epoch);

    void swap(lws_time_stamp &another)
    {
        std::swap(__micro_seconds_since_epoch, another.__micro_seconds_since_epoch);
    }

    std::string to_string() const;
    std::string to_formatted_string() const;

    bool valid() const;

    int64_t micro_seconds_since_epoch() const;
    //static——返回现在时间
    static lws_time_stamp now();
    //static——返回非法时间
    static lws_time_stamp invalid();

    bool operator<(const lws_time_stamp another)const ;

    bool operator==(const lws_time_stamp another)const ;
    
    lws_time_stamp operator-(lws_time_stamp another);

    static double time_difference(lws_time_stamp high, lws_time_stamp low);

    static lws_time_stamp add_time(lws_time_stamp time_stamp,double seconds);

    static const int k_micro_second_per_second = 1000 * 1000;
private:
    int64_t __micro_seconds_since_epoch;
};

#endif