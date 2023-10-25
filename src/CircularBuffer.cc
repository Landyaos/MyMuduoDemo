#include "CircularBuffer.h"

#include <errno.h>

CircularBuffer::~CircularBuffer()
{
}

size_t CircularBuffer::ReadFd(int fd)
{

    char extrabuf[65536];
    struct iovec vec[1];
    // size_t space = WriteableBytes();
    // if (read_index_ <= write_index_)
    // {
    //     vec[0].iov_base = buffer_.data() + write_index_;
    //     vec[0].iov_len = read_index_ == 0 ? (space - 1) : space - (read_index_ - 1);
    //     vec[1].iov_base = buffer_.data();
    //     vec[1].iov_len = read_index_ == write_index_ ? 0 : read_index_ - 1;
    // }
    // else
    // {
    //     vec[0].iov_base = buffer_.data() + write_index_;
    //     vec[0].iov_len = read_index_ - write_index_ - 1;
    //     vec[1].iov_base = nullptr;
    //     vec[1].iov_len = 0;
    // }
    vec[0].iov_base = extrabuf;
    vec[0].iov_len = sizeof(extrabuf);

    const ssize_t n = ::readv(fd, vec, 1);
    // if (n < 0)
    // {
    //     *saved_errno = errno;
    // }
    // else if (static_cast<size_t>(n) <= space)
    // {
    //     write_index_ = (write_index_ + n) % buffer_.size();
    // }
    // else
    // {
    //     write_index_ = (write_index_ + space) % buffer_.size();
    //     Append(extrabuf, n - space);
    // }

    Append(extrabuf, n);
    std::cout << "buffer size : " << buffer_.size() << std::endl;
    std::cout << n << std::endl;
    this->print();
    return n;
}
