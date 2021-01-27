#include "lwsTimeStamp.h"
#include <sys/time.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

static_assert(sizeof(lws_time_stamp) == sizeof(int64_t));

lws_time_stamp::lws_time_stamp()
    : __micro_seconds_since_epoch(0)
{
}

lws_time_stamp::lws_time_stamp(int64_t micro_seconds)
    : __micro_seconds_since_epoch(micro_seconds)
{
}

std::string lws_time_stamp::to_string() const
{
    char buf[32] = {0};
    int64_t seconds = __micro_seconds_since_epoch/k_micro_second_per_second;
    int64_t microseconds = __micro_seconds_since_epoch % k_micro_second_per_second;
    snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

std::string lws_time_stamp::to_formatted_string() const
{
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(__micro_seconds_since_epoch/k_micro_second_per_second);
    int microseconds = static_cast<int>(__micro_seconds_since_epoch % k_micro_second_per_second);
    tm tm_time;
  gmtime_r(&seconds, &tm_time);

  snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
      tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
      microseconds);
    return buf;
}

lws_time_stamp lws_time_stamp::now()
{
    timeval tv;
    gettimeofday(&tv,NULL);
    int64_t seconds = tv.tv_sec;
    return lws_time_stamp(seconds*k_micro_second_per_second+tv.tv_usec);
}

lws_time_stamp lws_time_stamp::invalid(){
    return lws_time_stamp();
}

bool lws_time_stamp::valid() const
{
    return __micro_seconds_since_epoch > 0;
}

int64_t lws_time_stamp::micro_seconds_since_epoch() const
{
    return __micro_seconds_since_epoch;
}

bool lws_time_stamp::operator<(const lws_time_stamp another)const
{
    return this->__micro_seconds_since_epoch < another.__micro_seconds_since_epoch;
}

bool lws_time_stamp::operator==(const lws_time_stamp another)const
{
    return this->__micro_seconds_since_epoch == another.__micro_seconds_since_epoch;
}

lws_time_stamp lws_time_stamp::operator-(lws_time_stamp another)
{
    return lws_time_stamp(
        this->__micro_seconds_since_epoch
        -another.__micro_seconds_since_epoch);
}

double lws_time_stamp::time_difference(lws_time_stamp high, lws_time_stamp low)
{
    int64_t diff = high.__micro_seconds_since_epoch - low.__micro_seconds_since_epoch;
    return static_cast<double>(diff) / k_micro_second_per_second;
}

lws_time_stamp lws_time_stamp::add_time(lws_time_stamp time_stamp, double seconds)
{
    int64_t inc = static_cast<int64_t>(seconds * k_micro_second_per_second);
    return lws_time_stamp(time_stamp.micro_seconds_since_epoch() + inc);
}
