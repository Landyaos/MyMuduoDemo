#include "HttpServer.h"

#include <iostream>
#include <string>

HttpServer::HttpServer(std::shared_ptr<EventLoop> event_loop, std::string ip, int port)
    : server_(event_loop, ip, port, "httpserver")
{
    server_.SetConnectionCallback(
        std::bind(&HttpServer::HandleConnection, this, std::placeholders::_1));
    server_.SetMessageCallback(
        std::bind(&HttpServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
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
    std::cout << "EchoServer - Connection" << std::endl;
    // std::string res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
    // conn->SendInLoop(res);
}

void HttpServer::HandleMessage(std::shared_ptr<TcpConnection> conn, std::string message, Timestamp time)
{
    HttpRequest http_request;
    HttpRequest::ParseRequestState parse_state = http_request.ParseRequest(message);
    HttpResponse http_response;
    // std::string res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
    // // http_request.toString();
    // http_response.set(res);
    conn->SendInLoop(message);
}
