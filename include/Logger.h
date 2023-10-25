#ifndef LOGGER_H
#define LOGGER_H

#include <functional>
#include <string>

#include "LogStream.h"
#include "Timestamp.h"

class Logger
{
public:
    enum LogLevel
    {
        TRACE = 0,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
    };

    Logger(std::string file_name, int line);
    Logger(std::string file_name, int line, LogLevel log_level);
    Logger(std::string file_name, int line, LogLevel log_level, std::string func);
    Logger(std::string file_name, int line, bool abort_flag);
    ~Logger();

    LogStream &stream();

    static LogLevel get_kLogLevel();
    static void set_kLogLevel(LogLevel log_level);

    static void setOutput(std::function<void(const char *msg, int len)> output_func);
    static void setFlush(std::function<void()> flush_func);

private:
    class Imple;
    Imple *imple_;
};

#define LOG_TRACE Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
#define LOG_DEBUG Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()

#endif