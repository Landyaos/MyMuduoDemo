#include "TcpConnection.h"

#include <netinet/in.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include "Logger.h"

TcpConnection::TcpConnection(std::shared_ptr<EventLoop> event_loop_ptr, const std::string& name, int sockfd, const sockaddr_in& local_addr, const sockaddr_in& client_addr)
    : event_loop_ptr_(event_loop_ptr),
      name_(name),
      state_(kConnecting),
      socket_(new Socket(sockfd)),
      tcp_connection_channel_(new Channel(sockfd)),
      local_addr_(local_addr),
      client_addr_(client_addr)
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

void TcpConnection::SendInLoop(std::string message)
{
    if (state_ == kConnected)
    {
        std::function<void()> sendFunction = std::bind(&TcpConnection::Send, this, std::move(message));
        event_loop_ptr_->RunInLoop(sendFunction);
    }
    else
    {
        LOG_WARN << "TCP CONNECTION IS DOWN, BUT SEND MESSAGE";
    }
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

void TcpConnection::ConnectionEstablished()
{
    set_state(kConnected);
    tcp_connection_channel_->EnableReading();
    event_loop_ptr_->AddChannel(tcp_connection_channel_);
    std::weak_ptr<TcpConnection> temp = shared_from_this();
    event_loop_ptr_->RunAt(Timestamp(2000), std::bind(&TcpConnection::HandleTimeout, this, temp));
    connection_cb_(shared_from_this());
}

void TcpConnection::ConnectionDestroyed()
{
    if (state_ == kConnected)
    {
        set_state(kDisconnected);
        connection_cb_(shared_from_this());
    }
}

void TcpConnection::HandleRead()
{
    // FIXED
    std::string buffer(1024, 0);
    Timestamp time;
    ssize_t ret = ::read(tcp_connection_channel_->fd(), const_cast<char*>(buffer.data()), 1023);
    if (ret > 0)
    {
        // read error
        message_cb_(shared_from_this(), buffer, time);
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
    std::string output;
    if (tcp_connection_channel_->isWriting())
    {
        ssize_t n = ::write(tcp_connection_channel_->fd(), output.data(), output.size());
        // Luo ji wanshan
        if (n > 0)
        {
            tcp_connection_channel_->DisableWritint();
            if (write_complete_cb_)
            {
                event_loop_ptr_->RunInLoop(std::bind(write_complete_cb_, shared_from_this()));
            }
            if (state_ == kDisconnecting)
            {
                ShutdownInLoop();
            }
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
    // log error
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

void TcpConnection::Send(std::string message)
{
    ssize_t wrote_len = 0;
    size_t remain_len = message.size();

    if (state_ == kDisconnected)
    {
        LOG_ERROR << "WARN DISCONNECTED";
        // LOG WARN DISCONNECTED
        return;
    }

    std::ostringstream response;
    std::string content = "Hello, World!";

    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: text/plain\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "\r\n";  // 空行分隔响应头和响应正文
    response << content;

    std::string httpResponse = response.str();
    std::cout << "执行到这里\n";
    ssize_t bytesSent = ::send(tcp_connection_channel_->fd(), httpResponse.c_str(), httpResponse.length(), 0);
    std::cout << "bytes sent " << bytesSent << httpResponse.size() << std::endl;
    std::cout << "执行结束\n";
    // while (remain_len > 0)
    // {
    //     // wrote_len = ::send(channel_->get_fd_(), message.c_str(), message.size(), 0);
    //     if (wrote_len < 0)
    //     {
    //         LOG_ERROR << "ERROR WroteLEN";
    //         // LOG ERROR
    //     }
    //     else
    //     {
    //         remain_len -= wrote_len;
    //     }
    //     std::cout << "remain len : " << remain_len << std::endl;
    // }
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
