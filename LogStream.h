#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include <string.h>

#include <string>

#include "noncopyable.h"

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template <int SIZE>
class FixedBuffer : noncopyable
{
public:
    FixedBuffer() : cur_(data_){};
    ~FixedBuffer() = default;

    void Append(const char *buf, size_t len)
    {
        // FIXME
        if (Available() > static_cast<int>(len))
        {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    };

    const char *Data() const { return data_; }

    int Length() const
    {
        return static_cast<int>(cur_ - data_);
    };
    char *Current()
    {
        return cur_;
    }
    int Available() const
    {
        return static_cast<int>(std::end(data_) - cur_);
    }
    void Add(size_t len)
    {
        cur_ += len;
    }

    void Clear()
    {
        memset(data_, 0, sizeof(data_));
        cur_ = data_;
    }

private:
    char data_[SIZE];
    char *cur_;
};

class LogStream : noncopyable
{
public:
    LogStream &operator<<(bool);
    LogStream &operator<<(short);
    LogStream &operator<<(unsigned short);
    LogStream &operator<<(int);
    LogStream &operator<<(unsigned int);
    LogStream &operator<<(long);
    LogStream &operator<<(unsigned long);
    LogStream &operator<<(long long);
    LogStream &operator<<(unsigned long long);
    LogStream &operator<<(float);
    LogStream &operator<<(double);
    LogStream &operator<<(char);
    LogStream &operator<<(const char *);
    LogStream &operator<<(const unsigned char *);
    LogStream &operator<<(const std::string &);
    const FixedBuffer<kSmallBuffer> &buffer() const { return buffer_; }

private:
    template <typename T>
    void Append(T);

    FixedBuffer<kSmallBuffer> buffer_;
    static const int kMaxNumericSize = 48;
};

#endif
