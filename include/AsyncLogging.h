#ifndef ASYNCLOGGING_H
#define ASYNCLOGGING_H

#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "LogStream.h"
#include "noncopyable.h"

class AsyncLogging : noncopyable
{
public:
    static AsyncLogging& getInstance();
    ~AsyncLogging();
    void Append(const char* log_line, int len);

private:
    AsyncLogging();
    void ThreadFunc();

    const int kFlushInterval_;
    std::atomic<bool> running_flag_;
    const std::string basename_;

    std::unique_ptr<std::thread> thread_ptr_;
    std::mutex mutex_;
    std::condition_variable condition_mutex_;
    std::mutex latch_;
    std::condition_variable contition_latch_;

    std::unique_ptr<FixedBuffer<kLargeBuffer>> current_buffer_;
    std::vector<std::unique_ptr<FixedBuffer<kLargeBuffer>>> buffers_vector_;
};

#endif