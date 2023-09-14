#include "TimerQueue.h"

TimerQueue::TimerQueue()
{
}

TimerQueue::~TimerQueue()
{
}

void TimerQueue::Acquire()
{
    // Timestamp now;
    // while (timers_queue.size() && timers_queue.top().expiration() <= now)
    // {
    //     // expired_timers_.emplace_back(std::make_shared(timers_queue.top()));
    // }
}

void TimerQueue::AddTimer(std::shared_ptr<Timer> timer_ptr)
{
}

void TimerQueue::DelTimer(std::shared_ptr<Timer> timer_ptr)
{
}

void TimerQueue::HandleTimerQueue()
{
    // expired_timers_.clear();
}
