#include "LogStream.h"

#include <stdint.h>
#include <stdio.h>

#include <algorithm>
#include <limits>
#include <string>

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;

template <typename T>
void LogStream::Append(T v)
{
    std::string temp = std::to_string(v);
    if (buffer_.Available() >= temp.size())
    {
        buffer_.Append(temp.c_str(), temp.size());
    }
}

LogStream &LogStream::operator<<(bool v)
{
    Append(v);
    return *this;
}

LogStream &LogStream::operator<<(short v)
{
    Append(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned short v)
{
    Append(v);
    return *this;
}

LogStream &LogStream::operator<<(int v)
{
    Append(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned int v)
{
    Append(v);
    return *this;
}

LogStream &LogStream::operator<<(long v)
{
    Append(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long v)
{
    Append(v);
    return *this;
}

LogStream &LogStream::operator<<(long long v)
{
    Append(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long long v)
{
    Append(v);
    return *this;
}

LogStream &LogStream::operator<<(float v)
{
    Append(v);
    return *this;
}

LogStream &LogStream::operator<<(double v)
{
    Append(v);
    return *this;
}

LogStream &LogStream::operator<<(char v)
{
    buffer_.Append(&v, 1);
    return *this;
}

LogStream &LogStream::operator<<(const char *v)
{
    if (v)
    {
        buffer_.Append(v, strlen(v));
    }
    else
    {
        buffer_.Append("(null)", 6);
    }
    return *this;
}

LogStream &LogStream::operator<<(const unsigned char *v)
{
    return operator<<(reinterpret_cast<const char *>(v));
}

LogStream &LogStream::operator<<(const std::string &v)
{
    buffer_.Append(v.c_str(), v.size());
    return *this;
}
