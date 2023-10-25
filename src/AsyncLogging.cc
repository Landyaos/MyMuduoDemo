#include "AsyncLogging.h"

#include <assert.h>

#include <chrono>
#include <iostream>

#include "LogFile.h"

AsyncLogging::AsyncLogging()
    : kFlushInterval_(3),
      basename_("./log"),
      running_flag_(false),
      current_buffer_(new FixedBuffer<kLargeBuffer>)
{
    current_buffer_->Clear();
    buffers_vector_.reserve(16);
    running_flag_ = true;
    std::unique_lock<std::mutex> latch_unique(latch_);
    thread_ptr_.reset(new std::thread(std::bind(&AsyncLogging::ThreadFunc, this), "Thread Func"));
    contition_latch_.wait(latch_unique);
}

AsyncLogging::~AsyncLogging()
{
    running_flag_ = false;
    condition_mutex_.notify_one();
    thread_ptr_->join();
}

AsyncLogging &AsyncLogging::getInstance()
{
    static AsyncLogging instance;
    return instance;
}

void AsyncLogging::Append(const char *log_line, int len)
{
    std::lock_guard<std::mutex> mutex_guard(mutex_);
    if (current_buffer_->Available() > len)
    {
        current_buffer_->Append(log_line, len);
    }
    else
    {
        buffers_vector_.emplace_back(std::move(current_buffer_));
        current_buffer_.reset(new FixedBuffer<kLargeBuffer>);
        current_buffer_->Append(log_line, len);
        condition_mutex_.notify_all();
    }
}

void AsyncLogging::ThreadFunc()
{
    assert(running_flag_ == true);
    LogFile output(basename_.c_str());

    std::vector<std::unique_ptr<FixedBuffer<kLargeBuffer>>> buffers_to_wirte;
    buffers_to_wirte.reserve(16);
    contition_latch_.notify_all();

    while (running_flag_)
    {
        assert(buffers_to_wirte.empty());

        {
            std::unique_lock<std::mutex> mutex_unique(mutex_);
            if (buffers_vector_.empty())
            {
                condition_mutex_.wait_for(mutex_unique, std::chrono::seconds(kFlushInterval_));
            }

            buffers_vector_.push_back(std::move(current_buffer_));
            current_buffer_.reset(new FixedBuffer<kLargeBuffer>);
            buffers_to_wirte.swap(buffers_vector_);
        }
        assert(!buffers_to_wirte.empty());

        for (const auto &buffer : buffers_to_wirte)
        {
            output.Append(buffer->Data(), buffer->Length());
        }

        buffers_to_wirte.clear();
        output.Flush();
    }
    output.Flush();
}
