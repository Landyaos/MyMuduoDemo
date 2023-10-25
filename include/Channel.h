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
    Channel(int fd);
    ~Channel();

    void HandleEvent(Timestamp receive_time);
    void set_read_call_back_(std::function<void()> read_call_back) { read_call_back_ = read_call_back; }
    void set_write_call_back_(std::function<void()> write_call_back) { write_call_back_ = write_call_back; }
    void set_close_call_back_(std::function<void()> close_call_back) { close_call_back_ = close_call_back; }
    void set_error_call_back_(std::function<void()> error_call_back) { error_call_back_ = error_call_back; }

    int fd() const { return fd_; }
    int watch_events() const { return watch_events_; }
    void set_trigger_events(int trigger_events) { trigger_events_ = trigger_events; }

    bool run_in_loop_flag() { return run_in_loop_flag_; }
    void set_run_in_loop_flag(bool run_in_loop_flag) { run_in_loop_flag_ = run_in_loop_flag; }
    void set_event_handling(bool event_handling) { event_handling_ = event_handling; }


    void EnableReading();
    void DisableReading();
    void EnableWriting();
    void DisableWritint();

    bool isWriting() const;
    bool isReading() const;

private:
    const int fd_;
    int watch_events_;
    int trigger_events_;

    bool event_handling_;
    bool run_in_loop_flag_;

    std::function<void()> read_call_back_;
    std::function<void()> write_call_back_;
    std::function<void()> close_call_back_;
    std::function<void()> error_call_back_;
};

#endif