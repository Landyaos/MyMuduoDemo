#ifndef LOGFILE_H
#define LOGFILE_H

#include <cstdio>

#include "noncopyable.h"

class LogFile : noncopyable
{
public:
    LogFile(const char *file_name, int flush_interval = 3, int flush_every_N = 1024);
    ~LogFile();
    void Append(const char *log_line, int len);
    void Flush();
private:
    FILE *fp_;
    off_t written_bytes_;
    char buffer_[64 * 1024];

    int count_;
    const int flush_every_N_;
    const int flush_interval_;
};

#endif