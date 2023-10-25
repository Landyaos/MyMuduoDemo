#include "TcpServer.h"

#include <arpa/inet.h>

#include <iostream>

TcpServer::TcpServer(std::shared_ptr<EventLoop> event_loop_ptr, const std::string ip, const int port, std::string name)
    : event_loop_ptr_(event_loop_ptr),
      name_(name),
      event_loop_thread_pool_(new EventLoopThreadPool(event_loop_ptr, name)),
      next_connection_(1)
{
    SetThreadNum(0);
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr_.sin_port = htons(port);
    acceptor_.reset(new Acceptor(event_loop_ptr_, server_addr_));
    acceptor_->set_connection_callback(std::bind(&TcpServer::HandleConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    for (auto &item : connection_map_)
    {
        std::shared_ptr<TcpConnection> conn = item.second.lock();
        if (conn)
        {
            conn->get_event_loop_ptr_()->RunInLoop(std::bind(&TcpConnection::ConnectionDestroyed, conn));
        }

        /**
         * @brief TODO
         *  waiting for that all the conn is disconnected.
         *
         */
    }
}

void TcpServer::SetThreadNum(int thread_num)
{
    event_loop_thread_pool_->set_thread_num_(thread_num);
}

void TcpServer::Start()
{
    acceptor_->Bind();
    acceptor_->Listen();
    event_loop_thread_pool_->Start(thread_init_cb_);
    event_loop_ptr_->RunInLoop(std::bind(&EventLoop::AddChannel, event_loop_ptr_.get(), acceptor_->accept_channel_ptr()));
}

void TcpServer::HandleConnection(int sockfd, const sockaddr_in client_addr)
{
    std::shared_ptr<EventLoop> io_loop = event_loop_thread_pool_->NextLoop();
    std::string name = inet_ntoa(client_addr.sin_addr) + std::to_string(client_addr.sin_port);
    std::cout << "new connnection : " << name << " bind epollfd" << io_loop->epollfd() << std::endl;
    std::shared_ptr<TcpConnection> conn(new TcpConnection(io_loop, name, sockfd, server_addr_, client_addr));
    connection_map_[name] = std::weak_ptr<TcpConnection>(conn);
    conn->set_connection_cb(connection_cb_);
    conn->set_message_cb(message_cb_);
    conn->set_write_complete_cb(write_complete_cb_);
    conn->set_close_cb(std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));

    io_loop->RunInLoop(std::bind(&TcpConnection::ConnectionEstablished, conn));
}

void TcpServer::RemoveConnection(const std::shared_ptr<TcpConnection> conn)
{
    event_loop_ptr_->RunInLoop(std::bind(&TcpServer::RemoveConnectionInLoop, this, conn));
}

void TcpServer::RemoveConnectionInLoop(const std::shared_ptr<TcpConnection> conn)
{
    connection_map_.erase(conn->get_name_());
    conn->ConnectionDestroyed();
}
