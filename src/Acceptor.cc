#include "Acceptor.h"

#include <error.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <memory>

Acceptor::Acceptor(std::shared_ptr<EventLoop> event_loop_ptr, const sockaddr_in server_addr)
    : event_loop_ptr_(event_loop_ptr),
      server_addr_(server_addr),
      acceptfd_(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP)),
      accept_channel_ptr_(new Channel(acceptfd_)),
      listen_flag_(false),
      idlefd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    if (acceptfd_ < 0)
    {
        // handle error
    }
    if (idlefd_ < 0)
    {
        // handle error
    }

    int optval = 1;
    if (::setsockopt(acceptfd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval))) < 0)
    {
        // handle error
    }
    if (::setsockopt(acceptfd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof(optval))) < 0)
    {
        // handle error
    }

    accept_channel_ptr_->set_read_call_back_(std::bind(&Acceptor::HandleReadEvent, this));
}

Acceptor::~Acceptor()
{
    event_loop_ptr_->RemoveChannel(accept_channel_ptr_);
    ::close(idlefd_);
}

void Acceptor::Bind()
{
    if (::bind(acceptfd_, (struct sockaddr*)&server_addr_, static_cast<socklen_t>(sizeof(server_addr_))) < 0)
    {
        // handle error
    }
}

void Acceptor::Listen()
{
    if (::listen(acceptfd_, SOMAXCONN) < 0)
    {
        // handle error
    }
    accept_channel_ptr_->EnableReading();
    listen_flag_ = true;
}

void Acceptor::HandleReadEvent()
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = static_cast<socklen_t>(sizeof(addr));
    int connfd = ::accept4(acceptfd_, (struct sockaddr*)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0)
    {
        if (connection_callback_)
        {
            connection_callback_(connfd, addr);
        }
        else if (::close(connfd) < 0)
        {
            // handle error
        }
    }
    // RUN OUT OF FD
    else if (errno == EMFILE)
    {
        ::close(idlefd_);
        idlefd_ = ::accept(acceptfd_, NULL, NULL);
        ::close(idlefd_);
        idlefd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
}
