#include "Echo.h"
#include "EventLoop.h"
#include "HttpServer.h"

int main()
{

    std::shared_ptr<EventLoop> event_loop_ptr(new EventLoop());
    event_loop_ptr->Init();
    HttpServer server(event_loop_ptr, "127.0.0.1", 10086);

    server.Start();
    event_loop_ptr->Loop();
}