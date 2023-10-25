#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

class Timestamp
{
public:
    // 使用高精度时钟来获取当前时间
    Timestamp();

    Timestamp(long long);

    // 获取时间戳的秒数
    long long seconds() const
    {
        return std::chrono::duration_cast<std::chrono::seconds>(time_point_.time_since_epoch()).count();
    }

    // 获取时间戳的毫秒数
    long long milliseconds() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(time_point_.time_since_epoch()).count();
    }

    // 获取时间戳的微秒数
    long long microseconds() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(time_point_.time_since_epoch()).count();
    }

    // 格式化时间戳为字符串
    std::string to_string() const
    {
        std::time_t time = std::chrono::system_clock::to_time_t(time_point_);
        std::tm tm = *std::localtime(&time);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(time_point_.time_since_epoch()).count() % 1000000;
        oss << "." << std::setw(6) << std::setfill('0') << microseconds;
        return oss.str();
    }

    // 重写比较运算符
    bool operator<(const Timestamp& other) const { return time_point_ < other.time_point_; }

    bool operator<=(const Timestamp& other) const { return time_point_ <= other.time_point_; }

    bool operator>(const Timestamp& other) const
    {
        return time_point_ > other.time_point_;
    }

    bool operator>=(const Timestamp& other) const
    {
        return time_point_ >= other.time_point_;
    }

    bool operator==(const Timestamp& other) const
    {
        return time_point_ == other.time_point_;
    }

    bool operator!=(const Timestamp& other) const
    {
        return time_point_ != other.time_point_;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> time_point_;
};

#endif