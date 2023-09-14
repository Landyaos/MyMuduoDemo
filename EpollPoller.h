#ifndef EPOLLER_H
#define EPOLLER_H

#include "Poller.h"

struct epoll_event;

class EpollPoller : public Poller
{
public:
    EpollPoller();
    ~EpollPoller();

    void Poll(std::vector<Channel *> &active_channels) override;
    void AddChannel(std::shared_ptr<Channel> channel_ptr) override;
    void UpdateChannel(std::shared_ptr<Channel> channel_ptr) override;
    void RemoveChannel(std::shared_ptr<Channel> channel_ptr) override;

private:
    void Update(int opreration, std::shared_ptr<Channel> channel_ptr);

    int epollfd_;
    std::vector<epoll_event> events_;
};

#endif