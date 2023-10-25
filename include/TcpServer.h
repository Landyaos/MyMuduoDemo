#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <map>
#include <string>

#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "TcpConnection.h"
#include "noncopyable.h"

class TcpServer : noncopyable
{
public:
    TcpServer(std::shared_ptr<EventLoop> event_loop_ptr,
              const std::string ip,
              const int port,
              std::string name);
    ~TcpServer();

    sockaddr_in GetAddr() { return server_addr_; }
    std::string GetName() { return name_; }
    std::shared_ptr<EventLoop> GetMainLoop() { return event_loop_ptr_; }

    void SetThreadNum(int thread_num);
    void SetThreadInitCallback(std::function<void(std::shared_ptr<EventLoop>)> cb) { thread_init_cb_ = cb; }
    std::shared_ptr<EventLoopThreadPool> GetThreadPool() { return event_loop_thread_pool_; }

    void Start();

    void SetConnectionCallback(std::function<void(std::shared_ptr<TcpConnection>)> cb) { connection_cb_ = cb; }
    void SetMessageCallback(std::function<void(std::shared_ptr<TcpConnection>)> cb) { message_cb_ = cb; }
    void SetWriteCompleteCallback(std::function<void(std::shared_ptr<TcpConnection>)> cb) { write_complete_cb_ = cb; }

private:
    void HandleConnection(int sockfd, const sockaddr_in server_addr_);
    void RemoveConnection(const std::shared_ptr<TcpConnection> conn);
    void RemoveConnectionInLoop(const std::shared_ptr<TcpConnection> conn);

    std::shared_ptr<EventLoop> event_loop_ptr_;
    struct sockaddr_in server_addr_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<EventLoopThreadPool> event_loop_thread_pool_;
    std::function<void(std::shared_ptr<TcpConnection>)> connection_cb_;
    std::function<void(std::shared_ptr<TcpConnection>)> message_cb_;
    std::function<void(std::shared_ptr<TcpConnection>)> write_complete_cb_;
    std::function<void(std::shared_ptr<EventLoop>)> thread_init_cb_;
    std::atomic<int> started_;
    int next_connection_;
    std::map<std::string, std::shared_ptr<TcpConnection>> connection_map_;
};

#endif