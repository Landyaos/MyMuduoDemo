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
      channel_(new Channel(event_loop_ptr, sockfd)),
      local_addr_(local_addr),
      client_addr_(client_addr)
{
    channel_->set_read_call_back_(
        std::bind(&TcpConnection::HandleRead, this));
    channel_->set_write_call_back_(
        std::bind(&TcpConnection::HandleWrite, this));
    channel_->set_close_call_back_(
        std::bind(&TcpConnection::HandleClose, this));
    channel_->set_error_call_back_(
        std::bind(&TcpConnection::HandleError, this));

    socket_->SetKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
}

void TcpConnection::Send(std::string message)
{
    if (state_ == kConnected)
    {
        if (event_loop_ptr_->isLoopThread())
        {
            LOG_INFO << "执行";
            SendInLoop(std::move(message));
        }
    }
    else
    {
        std::function<void()> sendFunction = std::bind(&TcpConnection::SendInLoop, this, std::move(message));
        event_loop_ptr_->RunInLoop(sendFunction);
    }
}

void TcpConnection::Shutdown()
{
    if (state_ == kConnected)
    {
        set_state_(kDisconnecting);
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
    set_state_(kConnected);
    channel_->Tie(shared_from_this());
    channel_->Add();
    connection_cb_(shared_from_this());
}

void TcpConnection::ConnectionDestroyed()
{
    if (state_ == kConnected)
    {
        set_state_(kDisconnected);
        connection_cb_(shared_from_this());
    }
    channel_->Remove();
}

void TcpConnection::HandleRead()
{
    // FIXED
    std::string buffer(1024, 0);
    Timestamp time;
    ssize_t ret = ::read(channel_->get_fd_(), const_cast<char*>(buffer.data()), 1023);
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
        // error
        HandleError();
    }
}

void TcpConnection::HandleWrite()
{
    if (channel_->isWriting())
    {
        ssize_t n = ::write(channel_->get_fd_(), output.data(), output.size());
        // Luojiwanshan
        if (n > 0)
        {
            channel_->DisableWritint();
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
    set_state_(kDisconnected);
    connection_close_cb_(shared_from_this());
    channel_->Remove();
}

void TcpConnection::HandleError()
{
    // log error
}

void TcpConnection::SendInLoop(std::string message)
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
    ssize_t bytesSent = ::send(channel_->get_fd_(), httpResponse.c_str(), httpResponse.length(), 0);
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
    if (!static_cast<bool>(channel_->isWriting()))
    {
        socket_->ShutdownWrite();
    }
}

void TcpConnection::StartReadInLoop()
{
    if (!read_flag_ || !static_cast<bool>(channel_->isWriting()))
    {
        channel_->EnableReading();
        read_flag_ = true;
    }
}

void TcpConnection::StopReadInLoop()
{
    if (read_flag_ || channel_->isReading())
    {
        channel_->DisableReading();
        read_flag_ = false;
    }
}
