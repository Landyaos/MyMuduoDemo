#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

#include "Echo.h"
#include "HttpServer.h"
#include "EventLoop.h"
#include "Logger.h"

void LogFunc()
{
    for (int i = 0; i < 10000; i++)
    {
        LOG_INFO << "hello";
        LOG_DEBUG << 1;
        LOG_FATAL << 2.5555555;
        LOG_TRACE << "12345";
    
    }
}

void LogTest()
{
    std::thread t(LogFunc);
    t.join();
}

int main()
{
    std::shared_ptr<EventLoop> event_loop_ptr(new EventLoop());
    event_loop_ptr->Init();
    HttpServer server(event_loop_ptr, "127.0.0.1", 10086);

    server.Start();
    event_loop_ptr->Loop();
}