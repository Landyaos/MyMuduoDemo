#include "EventLoopThreadPool.h"

#include <assert.h>

#include <iostream>
#include <mutex>

std::mutex mutex_;

EventLoopThreadPool::EventLoopThreadPool(std::shared_ptr<EventLoop> base_loop, const std::string& name)
    : base_loop_(base_loop),
      name_(name),
      start_flag_(false),
      thread_num_(0),
      next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    for (int i = 0; i < thread_num_; ++i)
    {
        loops_pool_[i]->Quit();
        threads_pool_[i]->join();
    }
}

void EventLoopThreadPool::Start(const std::function<void(std::shared_ptr<EventLoop>)> thread_init_callback)
{
    assert(!start_flag_);
    start_flag_ = true;

    for (int i = 0; i < thread_num_; ++i)
    {
        std::shared_ptr<std::thread> event_thread_ptr(new std::thread(&EventLoopThreadPool::ThreadFunc, this, thread_init_callback));
        threads_pool_.emplace_back(std::move(event_thread_ptr));
    }
    if (thread_num_ == 0 && thread_init_callback)
    {
        // thread_init_callback(base_loop_);
        loops_pool_.emplace_back(base_loop_);
    }
}

std::shared_ptr<EventLoop> EventLoopThreadPool::NextLoop()
{
    assert(start_flag_);
    auto loop = base_loop_;
    std::cout << " pool size " << loops_pool_.size() << " get " << next_ << std::endl;
    if (!loops_pool_.empty())
    {
        loop = loops_pool_[next_];
        next_ = (next_ + 1) % loops_pool_.size();
    }
    return loop;
}

std::shared_ptr<EventLoop> EventLoopThreadPool::LoopForHash(size_t hash_code)
{
    return std::shared_ptr<EventLoop>();
}

std::vector<std::shared_ptr<EventLoop>> EventLoopThreadPool::AllLoops()
{
    return loops_pool_;
}

void EventLoopThreadPool::ThreadFunc(const std::function<void(std::shared_ptr<EventLoop>)> thread_init_callback)
{
    std::shared_ptr<EventLoop> event_loop_ptr = std::make_shared<EventLoop>();
    event_loop_ptr->Init();
    {
        std::lock_guard<std::mutex> mutex_guard(mutex_);
        loops_pool_.emplace_back(event_loop_ptr);
    }
    // thread_init_callback(event_loop_ptr);

    event_loop_ptr->Loop();
}
