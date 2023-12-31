#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <netinet/in.h>
#include <sys/socket.h>

#include <functional>

#include "EventLoop.h"

class Acceptor : noncopyable
{
public:
    Acceptor(std::shared_ptr<EventLoop> event_loop_ptr, const sockaddr_in server_addr);
    ~Acceptor();
    void Bind();
    void Listen();
    void set_connection_callback(const std::function<void(int, const sockaddr_in)>& cb) { connection_callback_ = cb; }
    bool listen_flag() { return listen_flag_; }
    std::shared_ptr<Channel> accept_channel_ptr() { return accept_channel_ptr_; }

private:
    void HandleReadEvent();

    int idlefd_;
    int acceptfd_;
    bool listen_flag_;
    const sockaddr_in server_addr_;
    std::shared_ptr<EventLoop> event_loop_ptr_;
    std::shared_ptr<Channel> accept_channel_ptr_;
    std::function<void(int, const sockaddr_in)> connection_callback_;
};

#endif