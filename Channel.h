#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include <memory>

#include "noncopyable.h"

class EventLoop;
class Timestamp;

class Channel : noncopyable, public std::enable_shared_from_this<Channel>
{
public:
    Channel(std::shared_ptr<EventLoop> event_loop_ptr, int fd);
    ~Channel();

    void HandleEvent(Timestamp receive_time);
    void set_read_call_back_(std::function<void()> read_call_back)
    {
        read_call_back_ = read_call_back;
    }
    void set_write_call_back_(std::function<void()> write_call_back)
    {
        write_call_back_ = write_call_back;
    }
    void set_close_call_back_(std::function<void()> close_call_back)
    {
        close_call_back_ = close_call_back;
    }
    void set_error_call_back_(std::function<void()> error_call_back)
    {
        error_call_back_ = error_call_back;
    }

    void Tie(const std::shared_ptr<void> &);

    int get_fd_() const { return fd_; }
    int get_events_() const { return watch_events_; }
    void set_trigger_events_(int trigger_events)
    {
        trigger_events_ = trigger_events;
    }
    bool isNoneEvent() const { return watch_events_ == 0; }

    void EnableReading();

    void DisableReading();

    void EnableWriting();

    void DisableWritint();

    void DisableAll();

    bool isWriting() const;
    bool isReading() const;

    std::shared_ptr<EventLoop> get_owner_loop_() { return event_loop_ptr_; }

    void Add();
    
    void Remove();

private:
    void Update();

    const int fd_;
    std::shared_ptr<EventLoop> event_loop_ptr_;
    int watch_events_;
    int trigger_events_;

    bool log_hup_;

    std::weak_ptr<void> tie_;
    bool tied_;

    bool event_handling_;
    bool added_to_loop_;

    std::function<void()> read_call_back_;
    std::function<void()> write_call_back_;
    std::function<void()> close_call_back_;
    std::function<void()> error_call_back_;
};

#endif