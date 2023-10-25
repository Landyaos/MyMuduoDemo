#ifndef ECHO_H
#define ECHO_H

#include "TcpServer.h"

class Echo
{
public:
    Echo(std::shared_ptr<EventLoop> event_loop_ptr, std::string ip, int port);
    ~Echo();
    void Start();

private:
    void HandleConnection(std::shared_ptr<TcpConnection> conn);
    void HandleMessage(std::shared_ptr<TcpConnection> conn);

    TcpServer server_;
};

#endif