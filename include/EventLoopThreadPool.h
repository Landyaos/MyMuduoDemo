#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "EventLoop.h"
#include "noncopyable.h"

class EventLoopThreadPool : noncopyable
{
public:
    EventLoopThreadPool(std::shared_ptr<EventLoop> base_loop, const std::string& name);
    ~EventLoopThreadPool();
    void set_thread_num_(int thread_num) { thread_num_ = thread_num; }
    void Start(const std::function<void(std::shared_ptr<EventLoop>)> thread_init_callback);

    std::shared_ptr<EventLoop> NextLoop();

    std::shared_ptr<EventLoop> LoopForHash(size_t hash_code);

    std::vector<std::shared_ptr<EventLoop>> AllLoops();

    bool get_start_flag_() const { return start_flag_; }

    const std::string& get_name_() const { return name_; }

private:
    void ThreadFunc(const std::function<void(std::shared_ptr<EventLoop>)> thread_init_callback);

    std::shared_ptr<EventLoop> base_loop_;
    std::string name_;
    bool start_flag_;
    int thread_num_;
    int next_;

    std::vector<std::shared_ptr<EventLoop>> loops_pool_;
    std::vector<std::shared_ptr<std::thread>> threads_pool_;
};

#endif