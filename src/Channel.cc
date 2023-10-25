#include "Channel.h"

#include <assert.h>
#include <poll.h>
#include <sys/epoll.h>

#include <iostream>

#include "EventLoop.h"

Channel::Channel(int fd)
    : fd_(fd),
      watch_events_(0),
      trigger_events_(0),
      event_handling_(false),
      run_in_loop_flag_(false)
{
}

Channel::~Channel()
{
    assert(!event_handling_);
    assert(!run_in_loop_flag_);
}

void Channel::HandleEvent(Timestamp receive_time)
{
    event_handling_ = true;
    if ((trigger_events_ & EPOLLHUP) && !(trigger_events_ & EPOLLIN))
    {
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
        event_handling_ = false;
        return;
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

void Channel::EnableReading()
{
    watch_events_ |= (EPOLLIN | EPOLLPRI);
}

void Channel::DisableReading()
{
    watch_events_ &= ~(EPOLLIN | EPOLLPRI);
}

void Channel::EnableWriting()
{
    watch_events_ |= EPOLLOUT;
}

void Channel::DisableWritint()
{
    watch_events_ &= ~EPOLLOUT;
}

bool Channel::isWriting() const
{
    return watch_events_ & EPOLLOUT;
}

bool Channel::isReading() const
{
    return watch_events_ & (EPOLLIN | EPOLLPRI);
}
