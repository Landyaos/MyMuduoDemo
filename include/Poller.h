#ifndef POLLER_H
#define POLLER_H

#include <map>
#include <memory>
#include <vector>

#include "Channel.h"
#include "noncopyable.h"

class Poller : noncopyable
{
public:
    Poller() = default;
    virtual ~Poller() = default;

    virtual void Poll(std::vector<Channel *> &active_channels) = 0;

    virtual void AddChannel(std::shared_ptr<Channel> channel) = 0;

    virtual void UpdateChannel(std::shared_ptr<Channel> channel) = 0;

    virtual void RemoveChannel(std::shared_ptr<Channel> channel) = 0;

    virtual bool HasChannel(std::shared_ptr<Channel> channel) const;

protected:
    std::map<int, std::shared_ptr<Channel>> channels_map_;
};

#endif