#ifndef TIMEQUEUE_H
#define TIMEQUEUE_H

#include <memory>
#include <mutex>
#include <queue>
#include <unordered_set>
#include <vector>

#include "Timer.h"
#include "noncopyable.h"

class TimerSchedule : noncopyable
{
public:
    TimerSchedule();
    ~TimerSchedule();
    void Acquire();
    void AddTimer(std::shared_ptr<Timer> timer_ptr);
    void DelTimer(std::shared_ptr<Timer> timer_ptr);
    void HandleTimerQueue();

private:
    std::priority_queue<
        std::shared_ptr<Timer>,
        std::vector<std::shared_ptr<Timer>>,
        std::function<bool(std::shared_ptr<Timer>, std::shared_ptr<Timer>)>>
        timers_queue_{
            [](std::shared_ptr<Timer> a, std::shared_ptr<Timer> b)
            {
                return a->expiration() > b->expiration();
            }};
};

#endif
