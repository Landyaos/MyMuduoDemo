#include "EventLoop.h"

#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <assert.h>

#include <iostream>
#include <thread>

#include "Logger.h"

thread_local std::shared_ptr<EventLoop> kEventLoopTLS;

EventLoop::EventLoop()
    : loop_flag_(false),
      quit_flag_(false),
      event_handling_flag_(false),
      thread_id_(std::this_thread::get_id()),
      poller_ptr_(new EpollPoller()),
      wakeup_fd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
      timer_fd_(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC))
{
    if (timer_fd_ < 0)
        LOG_FATAL << "FAILED IN timerfd_create";
    if (wakeup_fd_ < 0)
        LOG_FATAL << "FAILED IN eventfd_create";
}

void EventLoop::Init()
{
    if (kEventLoopTLS)
        LOG_FATAL << "BUG IN kEventLoopTLS INVAILD";
    else
        kEventLoopTLS = shared_from_this();

    timer_queue_ptr_.reset(new TimerSchedule());
    timer_channel_.reset(new Channel(timer_fd_));
    timer_channel_->set_read_call_back_(std::bind(&EventLoop::HandleTimerEvent, this));
    // timer_channel_->EnableReading();
    // AddChannel(timer_channel_);

    wakeup_channel_.reset(new Channel(wakeup_fd_));
    wakeup_channel_->set_read_call_back_(std::bind(&EventLoop::HandleWakeUpEvent, this));
    wakeup_channel_->EnableReading();
    AddChannel(wakeup_channel_);
}

EventLoop::~EventLoop()
{
    RemoveChannel(timer_channel_);
    RemoveChannel(wakeup_channel_);
    ::close(wakeup_fd_);
    ::close(timer_fd_);
    kEventLoopTLS = nullptr;
}

void EventLoop::Loop()
{
    loop_flag_ = true;
    quit_flag_ = false;
    while (!quit_flag_)
    {
        Timestamp time;
        active_channel_list_.clear();
        assert(active_channel_list_.size() == 0);
        poller_ptr_->Poll(active_channel_list_);
        event_handling_flag_ = true;
        for (auto channel : active_channel_list_)
        {
            std::cout << "处理事件 sockfd: " << channel->fd() << std::endl;
            channel->HandleEvent(time);
            std::cout << "处理结束 sockfd: " << channel->fd() << std::endl;
        }
        event_handling_flag_ = false;
        DoPendingFunctors();
    }
    loop_flag_ = false;
}

void EventLoop::Quit()
{
    quit_flag_ = true;
    if (!isLoopThread())
    {
        Wakeup();
    }
}

void EventLoop::RunInLoop(std::function<void()> cb)
{
    if (isLoopThread())
    {
        cb();
    }
    else
    {
        
        QueueInLoop(std::move(cb));
    }
}

void EventLoop::QueueInLoop(std::function<void()> cb)
{
    {
        std::lock_guard<std::mutex> mutex_guard(mutex_);
        pending_functors_.push_back(std::move(cb));
    }
    if (!isLoopThread() || calling_pend_functors_flag_)
    {
        Wakeup();
    }
}

void EventLoop::RunAt(Timestamp time, std::function<void()> cb)
{
    // 获取过期timer
    std::cout << "add a new timer -------------------------" << std::endl;
    std::shared_ptr<Timer> timer_ptr(new Timer(cb, time));
    timer_queue_ptr_->AddTimer(timer_ptr);
    struct itimerspec expiration;
    expiration.it_value.tv_sec = 5; // 初始超时时间为 5 秒
    expiration.it_value.tv_nsec = 0;
    expiration.it_interval.tv_sec = 0; // 重复间隔为 2 秒
    expiration.it_interval.tv_nsec = 0;
    if (::timerfd_settime(timer_fd_, 0, &expiration, NULL) == -1)
    {
        perror("timerfd_settime");
    }
}

void EventLoop::Wakeup()
{
    uint64_t flag = 1;
    if (::write(wakeup_fd_, &flag, sizeof(flag)) != sizeof(flag))
    {
        // wake up failed
    }
    std::cout << "wake up fd " << wakeup_fd_ << std::endl;
}

void EventLoop::AddChannel(std::shared_ptr<Channel> channel_ptr)
{
    channel_ptr->set_run_in_loop_flag(true);
    poller_ptr_->AddChannel(channel_ptr);
}

void EventLoop::UpdateChannel(std::shared_ptr<Channel> channel_ptr)
{
    poller_ptr_->UpdateChannel(channel_ptr);
}

void EventLoop::RemoveChannel(std::shared_ptr<Channel> channel_ptr)
{
    channel_ptr->set_run_in_loop_flag(false);
    poller_ptr_->RemoveChannel(channel_ptr);
}

bool EventLoop::HasChannel(std::shared_ptr<Channel> channel_ptr)
{
    return poller_ptr_->HasChannel(channel_ptr);
}

void EventLoop::HandleWakeUpEvent()
{
    uint64_t flag = 1;
    ::read(wakeup_fd_, &flag, sizeof(flag));
}

void EventLoop::HandleTimerEvent()
{
    std::cout << "time happeen----------" << std::endl;
    uint64_t flag = 1;
    ::read(timer_fd_, &flag, sizeof(flag));
    timer_queue_ptr_->HandleTimerQueue();
}

void EventLoop::DoPendingFunctors()
{
    std::vector<std::function<void()>> functors;
    {
        std::lock_guard<std::mutex> mutex_gurad(mutex_);
        functors.swap(pending_functors_);
    }
    for (const auto &func : functors)
    {
        func();
    }
}
