#ifndef CircularBuffer_H
#define CircularBuffer_H

#include <assert.h>
#include <stdio.h>
#include <sys/uio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

class CircularBuffer
{
public:
    static const size_t kInitialSize = 15;

    explicit CircularBuffer(size_t initial_size = kInitialSize)
        : buffer_(initial_size),
          read_index_(0),
          write_index_(0)
    {
        assert(ReadableBytes() == 0);
        assert(WriteableBytes() == (initial_size - 1));
    }
    ~CircularBuffer();

    size_t ReadableBytes() const
    {
        if (read_index_ <= write_index_)
        {
            return write_index_ - read_index_;
        }
        else
        {
            return buffer_.size() - (read_index_ - write_index_);
        }
    }

    size_t WriteableBytes() const
    {
        return buffer_.size() - ReadableBytes() - 1;
    }

    std::string ParseLine()
    {
        std::vector<char> res;
        int checked_index = read_index_;
        while (checked_index != write_index_)
        {
            if (buffer_[checked_index] == '\n')
            {
                if (res.size() > 1 && res.back() == '\r')
                {
                    read_index_ = (read_index_ + res.size() + 1) % buffer_.size();
                    return std::string(res.begin(), res.end() - 1);
                }
                return std::string();
            }

            res.push_back(buffer_[checked_index]);

            if (++checked_index == buffer_.size())
            {
                checked_index = 0;
            }
        }
        return std::string();
    }

    void Append(const char *data, size_t len)
    {
        // if (WriteableBytes() < len)
        // {
        //     int pre_size = buffer_.size();
        //     buffer_.resize(buffer_.size() * 2 + len);
        //     if (read_index_ > write_index_)
        //     {
        //         std::copy(buffer_.begin(), buffer_.begin() + write_index_ - 1, buffer_.begin() + pre_size);
        //         write_index_ = write_index_ + pre_size;
        //     }
        // }
        // assert(WriteableBytes() >= len);
        // if (buffer_.size() - write_index_ >= len)
        // {
        //     std::copy(data, data + len, buffer_.begin() + write_index_);
        //     write_index_ += len;
        // }
        // else
        // {
        //     std::copy(data, data + buffer_.size() - write_index_, buffer_.begin() + write_index_);
        //     std::copy(data + buffer_.size() - write_index_, data + len, buffer_.begin());
        //     write_index_ = len - (buffer_.size() - write_index_);
        // }

        if (WriteableBytes() < len)
        {
            int pos = buffer_.size();
            buffer_.resize(buffer_.size() * 2 + len);
            if (read_index_ > write_index_)
            {
                for (int i = 0; i < write_index_; ++i)
                {
                    buffer_[pos + i] = buffer_[i];
                }
                write_index_ = pos + write_index_;
            }
        }
        assert(WriteableBytes() >= len);
        for (int i = 0; i < len; ++i)
        {
            buffer_[(write_index_ + i) % buffer_.size()] = data[i];
        }
        write_index_ = (write_index_ + len) % buffer_.size();
    }

    size_t ReadFd(int fd);

    struct iovec *ReadableSpace()
    {
        struct iovec vec[2];
        if (read_index_ <= write_index_)
        {
            vec[0].iov_base = buffer_.data() + read_index_;
            vec[0].iov_len = write_index_ - read_index_;
            vec[1].iov_base = buffer_.data();
            vec[1].iov_len = 0;
        }
        else
        {
            vec[0].iov_base = buffer_.data() + read_index_;
            vec[0].iov_len = buffer_.size() - read_index_;
            vec[1].iov_base = buffer_.data();
            vec[1].iov_len = write_index_;
        }
        return vec;
    }

    void ReadBytesBy(size_t size)
    {
        read_index_ = (read_index_ + size) % buffer_.size();
    }

    size_t read_index() const { return read_index_; }
    size_t write_index() const { return write_index_; }

    void print()
    {
        std::cout << "print \n";
        if (read_index_ <= write_index_)
        {
            for (int i = read_index_; i < write_index_; ++i)
            {
                std::cout << buffer_[i];
            }
        }
        else
        {
            for (int i = read_index_; i < write_index_ + buffer_.size(); ++i)
            {
                std::cout << buffer_[i % buffer_.size()];
            }
        }
        std::cout << "end \n";
    }

private:
    std::vector<char> buffer_;
    size_t read_index_;
    size_t write_index_;
};

#endif