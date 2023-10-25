#ifndef Buffer_H
#define Buffer_H

#include <assert.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

static const std::string kCRLF = "\r\n";

class Buffer
{
public:
    static const size_t kInitialSize = 512;

    explicit Buffer(size_t initial_size = kInitialSize)
        : buffer_(initial_size),
          read_index_(0),
          write_index_(0)
    {
        assert(ReadableBytes() == 0);
        assert(WriteableBytes() == initial_size);
    }
    ~Buffer() = default;

    std::string ReadBySize(size_t len)
    {
        return "";
    }

    std::string ReadByLine()
    {
        auto res = std::search(buffer_.begin() + read_index_, buffer_.begin() + write_index_, kCRLF.begin(), kCRLF.end());
        if (res == buffer_.end())
            return kCRLF;
        else
        {
            std::string target = std::string(buffer_.begin() + read_index_, res);
            read_index_ = static_cast<size_t>(std::distance(buffer_.begin(), res)) + 2;
            return target;
        }
    }

    void Append(const char *data, size_t len)
    {
        if (WriteableBytes() < len)
        {
            buffer_.resize(buffer_.size() * 2 + len);
        }
        assert(WriteableBytes() > len);
        std::copy(data, data + len, buffer_.data() + write_index_);
    }

    ssize_t ReadFd(int fd);
    ssize_t WriteFd(int fd);

    void Print()
    {
        std::cout << "\n print \n";
        std::cout << std::string(buffer_.begin() + read_index_, buffer_.begin() + write_index_);
        std::cout << "\n end \n";
    }

    void Reset()
    {
        read_index_ = 0;
        write_index_ = 0;
    }

    const char *peek() const { return buffer_.data() + read_index_; }

    size_t get_read_index() const { return read_index_; }
    size_t get_write_index() const { return write_index_; }

    size_t ReadableBytes() const
    {
        return write_index_ - read_index_;
    }
    size_t WriteableBytes() const
    {
        return buffer_.size() - write_index_;
    }

private:
    std::vector<char> buffer_;
    size_t read_index_;
    size_t write_index_;
};

#endif