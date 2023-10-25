#include "Buffer.h"

#include <sys/uio.h>
#include <unistd.h>

#include "Logger.h"

ssize_t Buffer::ReadFd(int fd)
{

    char extrabuf[65536];
    struct iovec vec[2];
    int space = WriteableBytes();
    vec[0].iov_base = buffer_.data() + write_index_;
    vec[0].iov_len = space;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = 65536;

    const ssize_t n = ::readv(fd, vec, 2);
    if (n < 0)
    {
        LOG_ERROR << "ERROR";
    }
    else if (static_cast<size_t>(n) <= space)
    {
        write_index_ += n;
    }
    else
    {
        write_index_ += space;
        Append(extrabuf, n - space);
    }

    std::cout << "read size : " << n << "\n";
    // Print();

    return n;
}

ssize_t Buffer::WriteFd(int fd)
{
    ssize_t n = ::write(fd, buffer_.data() + read_index_, ReadableBytes());
    if (n > 0)
    {
        read_index_ += n;
        if (read_index_ == write_index_)
        {
            Reset();
        }
    }
    return n;
}
