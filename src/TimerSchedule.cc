#include "TimerSchedule.h"

TimerSchedule::TimerSchedule()
{
}

TimerSchedule::~TimerSchedule()
{
}

void TimerSchedule::Acquire()
{
    // Timestamp now;
    // while (timers_queue.size() && timers_queue.top().expiration() <= now)
    // {
    //     // expired_timers_.emplace_back(std::make_shared(timers_queue.top()));
    // }
}

void TimerSchedule::AddTimer(std::shared_ptr<Timer> timer_ptr)
{
    timers_queue_.emplace(timer_ptr);
}

void TimerSchedule::DelTimer(std::shared_ptr<Timer> timer_ptr)
{
}

void TimerSchedule::HandleTimerQueue()
{
    Timestamp now;
    while (!timers_queue_.empty() && timers_queue_.top()->expiration() < now)
    {
        timers_queue_.top()->HandleEvent();
        timers_queue_.pop();
    }
}
