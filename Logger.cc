#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "AsyncLogging.h"

extern Logger::LogLevel kLogLevel;

class Logger::Imple
{
public:
    Imple(std::string file_name, int line, LogLevel level)
        : stream_(),
          level_(level),
          line_(line),
          file_name_(file_name)
    {
    }
    ~Imple() = default;
    void LogHeader()
    {
        std::stringstream log_line;
        log_line << " \n[" << level_ << "] ";

        auto now = std::chrono::high_resolution_clock::now();
        auto now_time_t = std::chrono::high_resolution_clock::to_time_t(std::chrono::high_resolution_clock::now());
        std::tm tm_struct = *std::localtime(&now_time_t);
        log_line << std::put_time(&tm_struct, "%Y:%m:%d:%H:%M:%S");
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        log_line << "." << std::setw(6) << std::setfill('0') << (microseconds % 1000000);
        log_line << "-" << file_name_ << "-" << line_ << " : ";
        stream_ << log_line.str();
    }

    LogStream stream_;
    LogLevel level_;
    int line_;
    std::string file_name_;
};

Logger::Logger(std::string file_name, int line) : imple_(new Imple(file_name, line, INFO))
{
}

Logger::Logger(std::string file_name, int line, LogLevel log_level) : imple_(new Imple(file_name, line, log_level))
{
}

Logger::Logger(std::string file_name, int line, LogLevel log_level, std::string func) : imple_(new Imple(file_name, line, log_level))
{
}

Logger::Logger(std::string file_name, int line, bool abort_flag) : imple_(new Imple(file_name, line, abort_flag ? FATAL : ERROR))
{
}

Logger::~Logger()
{
    const FixedBuffer<kSmallBuffer> &buf(stream().buffer());
    imple_->LogHeader();
    AsyncLogging::getInstance().Append(buf.Data(), buf.Length());
}

LogStream &Logger::stream()
{
    return imple_->stream_;
}
