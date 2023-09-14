#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "TcpServer.h"

class HttpServer : noncopyable
{
public:
    HttpServer(std::shared_ptr<EventLoop> event_loop, std::string ip, int port);
    ~HttpServer();

    void Start();

private:
    void HandleConnection(std::shared_ptr<TcpConnection> conn);
    void HandleMessage(std::shared_ptr<TcpConnection> conn, std::string message, Timestamp time);

    TcpServer server_;
};

#endif