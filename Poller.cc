#include "Poller.h"

bool Poller::HasChannel(std::shared_ptr<Channel> channel) const
{
    auto it = channels_map_.find(channel->get_fd_());
    return it != channels_map_.end() && it->second == channel;
}
