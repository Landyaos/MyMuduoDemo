#ifndef TIMEQUEUE_H
#define TIMEQUEUE_H

#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <vector>

#include "Timer.h"
#include "noncopyable.h"

class TimerQueue : noncopyable
{
public:
    TimerQueue();
    ~TimerQueue();
    void Acquire();
    void AddTimer(std::shared_ptr<Timer> timer_ptr);
    void DelTimer(std::shared_ptr<Timer> timer_ptr);
    void HandleTimerQueue();

private:
    // std::vector<std::shared_ptr<Timer>> expired_timers_;
    // std::priority_queue<Timer, std::vector<Timer>, std::function<bool(Timer &, Timer &)>> timers_queue{
    //     [](Timer &a, Timer &b)
    //     {
    //         return a.expiration() > b.expiration();
    //     }};
};

#endif
