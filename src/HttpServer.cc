#include "HttpServer.h"

#include <iostream>
#include <string>

HttpServer::HttpServer(std::shared_ptr<EventLoop> event_loop, std::string ip, int port)
    : server_(event_loop, ip, port, "httpserver")
{
    server_.SetConnectionCallback(
        std::bind(&HttpServer::HandleConnection, this, std::placeholders::_1));
    server_.SetMessageCallback(
        std::bind(&HttpServer::HandleMessage, this, std::placeholders::_1));
}

HttpServer::~HttpServer()
{
}

void HttpServer::Start()
{
    server_.Start();
}

void HttpServer::HandleConnection(std::shared_ptr<TcpConnection> conn)
{
    // std::cout << "EchoServer - Connection" << std::endl;
}

void HttpServer::HandleMessage(std::shared_ptr<TcpConnection> conn)
{
    thread_local static HttpRequest request;
    request.ParseRequest(conn->get_input_buffer_ptr().get());
    // request.toString();
    // if (request.ParseState())
    // {
    //     HttpResponse response;
    //     response.set_data("hello world");
    //     response.set_version("HTTP/1.1");
    //     response.set_status("200");
    //     response.set_header("Content-Length", "11");
    //     response.set_header("Content-Type", "*/*");
    //     request.Reset();
        // std::string res = response.BuildHttpResponse();
        std::string res = "HTTP/1.1 200 OK\r\n"
                               "Date: Sun, 24 Oct 2023 12:00:00 GMT\r\n"
                               "Server: MyServer\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: 123\r\n"
                               "\r\n"
                               "<!DOCTYPE html>\r\n"
                               "<html>\r\n"
                               "<head>\r\n"
                               "    <title>Sample Response</title>\r\n"
                               "</head>\r\n"
                               "<body>\r\n"
                               "    <p>This is the response body.</p>\r\n"
                               "</body>\r\n"
                               "</html>\r\n";
        conn->SendInLoop(res.data(), res.size());
    // }
}
