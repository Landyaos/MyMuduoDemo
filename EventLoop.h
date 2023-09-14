#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "Channel.h"
#include "EpollPoller.h"
#include "TimerQueue.h"
#include "Timestamp.h"
#include "noncopyable.h"

class EventLoop : noncopyable, public std::enable_shared_from_this<EventLoop>
{
public:
    EventLoop();
    ~EventLoop();

    void Init();
    void Loop();
    void Quit();

    void RunInLoop(std::function<void()> cb);
    void QueueInLoop(std::function<void()> cb);

    void RunAt(Timestamp time, std::function<void()> cb);
    void RunAfter(Timestamp delay, std::function<void()> cb);
    void RunEvery(Timestamp interval, std::function<void()> cb);
    void DeleteTimer(std::unique_ptr<Timer> timer_ptr);

    void Wakeup();
    void AddChannel(std::shared_ptr<Channel> channel_ptr);
    void UpdateChannel(std::shared_ptr<Channel> channel_ptr);
    void RemoveChannel(std::shared_ptr<Channel> channel_ptr);
    bool HasChannel(std::shared_ptr<Channel> channel_ptr);

    bool isEventHandling() const { return event_handling_flag_; }
    bool isLoopThread() const { return thread_id_ == std::this_thread::get_id(); }

private:
    void HandleWakeUpEvent();
    void HandleTimerEvent();
    void DoPendingFunctors();

    bool loop_flag_;
    bool event_handling_flag_;
    bool calling_pend_functors_flag_;
    std::atomic<bool> quit_flag_;
    std::thread::id thread_id_;

    std::unique_ptr<EpollPoller> poller_ptr_;

    int wakeup_fd_;
    std::shared_ptr<Channel> wakeup_channel_;

    int timer_fd_;
    std::shared_ptr<Channel> timer_channel_;
    std::unique_ptr<TimerQueue> timer_queue_ptr_;

    std::vector<Channel*> active_channel_list_;

    std::mutex mutex_;
    std::vector<std::function<void()>> pending_functors_;
};

#endif