#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include <memory>
#include <string>

#include "Buffer.h"
#include "EventLoop.h"
#include "Socket.h"

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(std::shared_ptr<EventLoop> event_loop_ptr,
                  const std::string& name,
                  int sockfd,
                  const sockaddr_in& local_addr,
                  const sockaddr_in& client_addr);
    ~TcpConnection();

    std::shared_ptr<EventLoop> get_event_loop_ptr_() const { return event_loop_ptr_; }
    const std::string& get_name_() const { return name_; }
    const sockaddr_in& get_local_addr_() const { return local_addr_; }
    const sockaddr_in& get_client_addr() const { return client_addr_; }
    bool isConnected() const { return state_ == kConnected; }
    bool isDisconnected() const { return state_ == kDisconnected; }

    void Send(std::string message);
    // void Send(Buffer&& message);

    void Shutdown();
    void Close();
    void CloseWithDelay(double seconds);

    void SetTcpNoDelay();

    void StartRead();
    void StopRead();
    bool isReading() const { return read_flag_; }

    void set_connection_cb_(std::function<void(std::shared_ptr<TcpConnection>)> connection_cb)
    {
        connection_cb_ = connection_cb;
    }

    void set_message_cb_(std::function<void(std::shared_ptr<TcpConnection>, std::string, Timestamp)> message_cb)
    {
        message_cb_ = message_cb;
    }

    void set_write_complete_cb_(std::function<void(std::shared_ptr<TcpConnection>)> write_complete_cb)
    {
        write_complete_cb_ = write_complete_cb;
    }

    void set_close_cb_(std::function<void(std::shared_ptr<TcpConnection>)> close_cb)
    {
        connection_close_cb_ = close_cb;
    }

    std::shared_ptr<Buffer> get_input_buffer_ptr_() { return input_buffer_ptr_; }
    std::shared_ptr<Buffer> get_output_buffer_ptr_() { return output_buffer_ptr_; }

    void ConnectionEstablished();
    void ConnectionDestroyed();

private:
    enum ConnectionState
    {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting,
    };
    void HandleRead();
    void HandleWrite();
    void HandleClose();
    void HandleError();

    // void SendInLoop(Buffer&& message);
    void SendInLoop(std::string message);
    void ShutdownInLoop();

    void ForceCloseInLoop();
    void set_state_(ConnectionState state) { state_ = state; }

    void StartReadInLoop();
    void StopReadInLoop();

    std::shared_ptr<EventLoop> event_loop_ptr_;
    std::string name_;
    ConnectionState state_;
    bool read_flag_;

    std::shared_ptr<Socket> socket_;
    std::shared_ptr<Channel> channel_;

    const sockaddr_in local_addr_;
    const sockaddr_in client_addr_;

    std::function<void(std::shared_ptr<TcpConnection>)> connection_cb_;
    std::function<void(std::shared_ptr<TcpConnection>, std::string, Timestamp)> message_cb_;
    std::function<void(std::shared_ptr<TcpConnection>)> write_complete_cb_;
    std::function<void(std::shared_ptr<TcpConnection>)> high_water_mark_cb_;
    std::function<void(std::shared_ptr<TcpConnection>)> connection_close_cb_;

    std::shared_ptr<Buffer> input_buffer_ptr_;
    std::shared_ptr<Buffer> output_buffer_ptr_;
};

#endif