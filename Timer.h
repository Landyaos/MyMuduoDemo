#ifndef TIMER_H
#define TIMER_H

#include <functional>

#include "Timestamp.h"
#include "noncopyable.h"

class Timer : noncopyable
{
public:
    Timer(std::function<void()> callback, Timestamp expiration, long long duration, int repeat_num);
    Timer(std::function<void()> callback, Timestamp expiration);
    ~Timer() = default;
    Timestamp expiration() const { return expiration_; }
    void set_expiration(Timestamp expiration) { expiration_ = expiration; }
    void HandleEvent();

    bool operator<(const Timer& ohter) const { return expiration_ < ohter.expiration(); }
    bool operator<=(const Timer& ohter) const { return expiration_ <= ohter.expiration(); }
    bool operator>(const Timer& other) const { return expiration_ > other.expiration(); }
    bool operator>=(const Timer& other) const { return expiration_ >= other.expiration(); }

private:
    Timestamp expiration_;
    long long duration_;
    int repeat_num_;
    std::function<void()> callback_;
};

#endif