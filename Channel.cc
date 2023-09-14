#include "Channel.h"

#include <assert.h>
#include <poll.h>
#include <sys/epoll.h>

#include <iostream>

#include "EventLoop.h"

Channel::Channel(std::shared_ptr<EventLoop> event_loop_ptr, int fd)
    : event_loop_ptr_(event_loop_ptr),
      fd_(fd),
      watch_events_(0),
      trigger_events_(0),
      log_hup_(true),
      tied_(false),
      event_handling_(false),
      added_to_loop_(false)
{
}

Channel::~Channel()
{
    assert(!event_handling_);
    assert(!added_to_loop_);
}

void Channel::HandleEvent(Timestamp receive_time)
{
    event_handling_ = true;
    if ((trigger_events_ & EPOLLHUP) && !(trigger_events_ & EPOLLIN))
    {
        std::cout << "huo" << std::endl;
        if ((log_hup_))
        {
            // LOG
        }
        if (close_call_back_)
        {
            close_call_back_();
        }
    }

    if (trigger_events_ & EPOLLERR)
    {
        std::cout << "error" << std::endl;
        if (error_call_back_)
            error_call_back_();
    }

    if ((trigger_events_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)))
    {
        std::cout << "read in" << std::endl;
        if (read_call_back_)
            read_call_back_();
    }

    if (trigger_events_ & EPOLLOUT)
    {
        std::cout << "wirte out" << std::endl;
        if (write_call_back_)
            write_call_back_();
    }
    event_handling_ = false;
}

void Channel::Tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    tied_ = true;
}

void Channel::EnableReading()
{
    watch_events_ |= (EPOLLIN | EPOLLPRI);
    Update();
}

void Channel::DisableReading()
{
    watch_events_ &= ~(EPOLLIN | EPOLLPRI);
    Update();
}

void Channel::EnableWriting()
{
    watch_events_ |= EPOLLOUT;
    Update();
}

void Channel::DisableWritint()
{
    watch_events_ &= ~EPOLLOUT;
    Update();
}


bool Channel::isWriting() const
{
    return watch_events_ & EPOLLOUT;
}

bool Channel::isReading() const
{
    return watch_events_ & (EPOLLIN | EPOLLPRI);
}

void Channel::Add()
{
    added_to_loop_ = true;
    watch_events_ |= (EPOLLIN | EPOLLPRI);
    event_loop_ptr_->AddChannel(shared_from_this());
}

void Channel::Remove()
{
    added_to_loop_ = false;
    event_loop_ptr_->RemoveChannel(shared_from_this());
}

void Channel::Update()
{
    event_loop_ptr_->UpdateChannel(shared_from_this());
}
