#include "Socket.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

#include "Logger.h"

Socket::Socket(int sockfd) : sockfd_(sockfd) {}

Socket::~Socket()
{
    if (::close(sockfd_) < 0)
    {
        // socket close failed
    }
}

void Socket::Bind(const sockaddr_in addr)
{
    if (::bind(sockfd_, (struct sockaddr*)&addr, static_cast<socklen_t>(sizeof(addr))) < 0)
    {
        // bind error;
    }
}

void Socket::Listen()
{
    if (::listen(sockfd_, SOMAXCONN) < 0)
    {
        // listen error
    }
}

void Socket::Accept()
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = static_cast<socklen_t>(sizeof(addr));
    int connfd = ::accept4(sockfd_, (struct sockaddr*)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0)
    {
    }
    else
    {
        // accept fialed
    }
}

void Socket::ShutdownWrite()
{
    if (::shutdown(sockfd_, SHUT_WR) < 0)
    {
        // FAILED
    }
}

void Socket::SetTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof(optval)));
}

void Socket::SetReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval)));
    // SUCCESS CHECK
}

void Socket::SetReusePort(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof(optval)));
    if (ret < 0 && on)
    {
        // FAILED
    }
}

void Socket::SetKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof(optval)));
}