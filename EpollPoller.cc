#include "EpollPoller.h"

#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

#include "Logger.h"

const int kEventInitSize = 16;

EpollPoller::EpollPoller()
    : Poller(),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kEventInitSize)
{
    if (epollfd_ < 0)
    {
        // LOG ERROR
    }
}

EpollPoller::~EpollPoller()
{
    ::close(epollfd_);
}

void EpollPoller::Poll(std::vector<Channel *> &active_channels)
{
    int num_events = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), -1);
    int saved_errno = errno;

    if (num_events > 0)
    {
        // LOG
        std::cout << "Event Happen " << num_events << std::endl;
        for (int i = 0; i < num_events; ++i)
        {
            Channel *channel_ptr = static_cast<Channel *>(events_[i].data.ptr);
            //
            channel_ptr->set_trigger_events_(events_[i].events);
            active_channels.push_back(channel_ptr);
            std::cout << "sockfd" << channel_ptr->get_fd_() << std::endl;
        }

        if (num_events == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (num_events == 0)
    {
    }
    else
    {
        if (saved_errno != EINTR)
        {
            errno = saved_errno;
        }
    }
}

void EpollPoller::AddChannel(std::shared_ptr<Channel> channel_ptr)
{
    if (channels_map_.find(channel_ptr->get_fd_()) != channels_map_.end())
    {
        LOG_ERROR << "add channel failed, sock fd exist.";
    }
    channels_map_[channel_ptr->get_fd_()] = channel_ptr;
    Update(EPOLL_CTL_ADD, channel_ptr);
}

void EpollPoller::UpdateChannel(std::shared_ptr<Channel> channel_ptr)
{
    Update(EPOLL_CTL_MOD, channel_ptr);
}

void EpollPoller::RemoveChannel(std::shared_ptr<Channel> channel_ptr)
{
    Update(EPOLL_CTL_DEL, channel_ptr);
    channels_map_.erase(channel_ptr->get_fd_());
}

void EpollPoller::Update(int operation, std::shared_ptr<Channel> channel_ptr)
{
    struct epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.events = channel_ptr->get_events_();
    event.data.ptr = channel_ptr.get();

    int ret = ::epoll_ctl(epollfd_, operation, channel_ptr->get_fd_(), &event);
    if (ret < 0)
        std::cout << "epoll update error " << errno << " " << ret << std::endl;
    else
        std::cout << "epoll update epollfd " << epollfd_ << " operation  " << operation << " channel sockfd  " << channel_ptr->get_fd_() << std::endl;
}
