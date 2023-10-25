#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>

#include "noncopyable.h"


class Socket : noncopyable
{
public:
    explicit Socket(int sockfd);
    ~Socket();

    int get_sockfd_() { return sockfd_; }

    void Bind(const sockaddr_in addr);

    void Listen();

    void Accept();

    void ShutdownWrite();

    void SetTcpNoDelay(bool on);

    void SetReuseAddr(bool on);

    void SetReusePort(bool on);

    void SetKeepAlive(bool on);

private:
    const int sockfd_;
};

#endif