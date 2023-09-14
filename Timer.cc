#include "Timer.h"


Timer::Timer(std::function<void()> callback, Timestamp expiration, long long duration, int repeat_num)
    : callback_(callback),
      expiration_(expiration),
      duration_(duration),
      repeat_num_(repeat_num)
{
}
Timer::Timer(std::function<void()> callback, Timestamp expiration)
    : callback_(callback),
      expiration_(expiration),
      duration_(0),
      repeat_num_(0)
{
}
