
#include "timestamp.h"

Timestamp::Timestamp()
{
    second_since_epoch_ = time(nullptr);
}

Timestamp::Timestamp(int64_t second_since_epoch_): second_since_epoch_(second_since_epoch_)
{

}

// 当前时间。
Timestamp Timestamp::now() 
{ 
    return {};
}

time_t Timestamp::to_int() const
{
    return second_since_epoch_;
}

std::string Timestamp::to_string() const
{
    char buf[32] = {0};
    tm *tm_time = localtime(&second_since_epoch_);
    snprintf(buf, 20, "%4d-%02d-%02d %02d:%02d:%02d",
             tm_time->tm_year + 1900,
             tm_time->tm_mon + 1,
             tm_time->tm_mday,
             tm_time->tm_hour,
             tm_time->tm_min,
             tm_time->tm_sec);
    return buf;
}