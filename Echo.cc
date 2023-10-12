#include "Echo.h"

#include <iostream>

Echo::Echo(std::shared_ptr<EventLoop> event_loop_ptr, std::string ip, int port)
    : server_(event_loop_ptr, ip, port, "EchoServer")
{
    server_.SetConnectionCallback(
        std::bind(&Echo::HandleConnection, this, std::placeholders::_1));
    server_.SetMessageCallback(
        std::bind(&Echo::HandleMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

Echo::~Echo() = default;

void Echo::Start()
{
    server_.Start();
}

void Echo::HandleConnection(std::shared_ptr<TcpConnection> conn)
{
    std::cout << "EchoServer - Connection" << std::endl;
}

void Echo::HandleMessage(std::shared_ptr<TcpConnection> conn, std::string message, Timestamp time)
{
    std::cout << "EchoServer - Receive : " << message << std::endl;

    
    conn->SendInLoop(message);
}
