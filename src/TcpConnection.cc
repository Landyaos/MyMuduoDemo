#include "TcpConnection.h"

#include <netinet/in.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include "Logger.h"

TcpConnection::TcpConnection(std::shared_ptr<EventLoop> event_loop_ptr, const std::string &name, int sockfd, const sockaddr_in &local_addr, const sockaddr_in &client_addr)
    : event_loop_ptr_(event_loop_ptr),
      name_(name),
      state_(kConnecting),
      socket_(new Socket(sockfd)),
      tcp_connection_channel_(new Channel(sockfd)),
      local_addr_(local_addr),
      client_addr_(client_addr),
      input_buffer_ptr_(new Buffer),
      output_buffer_ptr_(new Buffer)
{
    tcp_connection_channel_->set_read_call_back_(std::bind(&TcpConnection::HandleRead, this));
    tcp_connection_channel_->set_write_call_back_(std::bind(&TcpConnection::HandleWrite, this));
    tcp_connection_channel_->set_close_call_back_(std::bind(&TcpConnection::HandleClose, this));
    tcp_connection_channel_->set_error_call_back_(std::bind(&TcpConnection::HandleError, this));

    socket_->SetKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
}

void TcpConnection::SendInLoop(const char *data, size_t len)
{
    assert(state_ == kConnected);
    event_loop_ptr_->RunInLoop(
        std::bind(&TcpConnection::Send, this, data, len));
}

void TcpConnection::Shutdown()
{
    if (state_ == kConnected)
    {
        set_state(kDisconnecting);
        event_loop_ptr_->RunInLoop(std::bind(&TcpConnection::ShutdownInLoop, this));
    }
}

void TcpConnection::SetTcpNoDelay()
{
    socket_->SetTcpNoDelay(true);
}

void TcpConnection::StartRead()
{
    event_loop_ptr_->RunInLoop(std::bind(&TcpConnection::StartReadInLoop, this));
}

void TcpConnection::StopRead()
{
    event_loop_ptr_->RunInLoop(std::bind(&TcpConnection::StopReadInLoop, this));
}

void TcpConnection::set_connection_cb(std::function<void(std::shared_ptr<TcpConnection>)> connection_cb)
{
    connection_cb_ = connection_cb;
}

void TcpConnection::set_message_cb(std::function<void(std::shared_ptr<TcpConnection>)> message_cb)
{
    message_cb_ = message_cb;
}

void TcpConnection::set_write_complete_cb(std::function<void(std::shared_ptr<TcpConnection>)> write_complete_cb)
{
    write_complete_cb_ = write_complete_cb;
}

void TcpConnection::set_close_cb(std::function<void(std::shared_ptr<TcpConnection>)> close_cb)
{
    connection_close_cb_ = close_cb;
}

void TcpConnection::ConnectionEstablished()
{
    set_state(kConnected);
    tcp_connection_channel_->EnableReading();
    event_loop_ptr_->AddChannel(tcp_connection_channel_);
    std::cout << "connection established success !\n";
    // std::weak_ptr<TcpConnection> temp = shared_from_this();
    // event_loop_ptr_->RunAt(Timestamp(2000), std::bind(&TcpConnection::HandleTimeout, this, temp));
    connection_cb_(shared_from_this());
}

void TcpConnection::ConnectionDestroyed()
{
    // HandleClose();
}

void TcpConnection::HandleRead()
{
    // FIXED
    ssize_t ret = input_buffer_ptr_->ReadFd(tcp_connection_channel_->fd());
    if (ret > 0)
    {
        message_cb_(shared_from_this());
        HandleClose();
    }
    else if (ret == 0)
    {
        HandleClose();
    }
    else
    {
        HandleError();
    }
}

void TcpConnection::HandleWrite()
{
    assert(tcp_connection_channel_->isWriting());
    ssize_t n = output_buffer_ptr_->WriteFd(tcp_connection_channel_->fd());
    // Luo ji wanshan
    if (n > 0)
    {
        if (output_buffer_ptr_->ReadableBytes() == 0)
        {
            tcp_connection_channel_->DisableWritint();
            event_loop_ptr_->UpdateChannel(tcp_connection_channel_);
            write_complete_cb_(shared_from_this());
        }
    }
}

void TcpConnection::HandleClose()
{
    set_state(kDisconnected);
    tcp_connection_channel_->set_event_handling(false);
    event_loop_ptr_->RemoveChannel(tcp_connection_channel_);
    connection_close_cb_(shared_from_this());
}

void TcpConnection::HandleError()
{
    set_state(kDisconnected);
    tcp_connection_channel_->set_event_handling(false);
    event_loop_ptr_->RemoveChannel(tcp_connection_channel_);
    connection_close_cb_(shared_from_this());
}

void TcpConnection::HandleTimeout(std::weak_ptr<TcpConnection> myself)
{
    if (auto myshare = myself.lock())
    {
        std::cout << "hhhhhhh" << std::endl;
    }
    else
    {
        std::cout << "eeeeeeee" << std::endl;
    }
}

void TcpConnection::Send(const char *data, size_t len)
{
    if (output_buffer_ptr_->ReadableBytes() > 0)
    {
        output_buffer_ptr_->Append(data, len);
    }
    else
    {
        ssize_t bytesSent = ::send(tcp_connection_channel_->fd(), data, len, 0);
        if (bytesSent < len)
        {
            if (output_buffer_ptr_->WriteableBytes() == 0)
                tcp_connection_channel_->EnableWriting();
            output_buffer_ptr_->Append(data + bytesSent, len - bytesSent);
        }
    }
}

void TcpConnection::ShutdownInLoop()
{
    if (!static_cast<bool>(tcp_connection_channel_->isWriting()))
    {
        socket_->ShutdownWrite();
    }
}

void TcpConnection::StartReadInLoop()
{
    if (!read_flag_ || !static_cast<bool>(tcp_connection_channel_->isWriting()))
    {
        tcp_connection_channel_->EnableReading();
        read_flag_ = true;
    }
}

void TcpConnection::StopReadInLoop()
{
    if (read_flag_ || tcp_connection_channel_->isReading())
    {
        tcp_connection_channel_->DisableReading();
        read_flag_ = false;
    }
}
