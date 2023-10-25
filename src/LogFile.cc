#include "LogFile.h"

#include <assert.h>

LogFile::LogFile(const char* file_name, int flush_interval, int flush_every_N)
    : fp_(::fopen(file_name, "ae")),
      flush_interval_(flush_interval),
      flush_every_N_(flush_every_N),
      count_(0)
{
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof(buffer_));
}

LogFile::~LogFile()
{
    ::fclose(fp_);
};

void LogFile::Append(const char* log_line, int len)
{
    size_t written = 0;
    while (written != len)
    {
        size_t remain = len - written;
        size_t n = ::fwrite_unlocked(log_line + written, 1, remain, fp_);
        if (n != remain)
        {
            int err = ::ferror(fp_);
            if (err)
            {
                ::fprintf(stderr, "Append File Failed ! \n");
                break;
            }
        }
        written += n;
    }
    ++count_;
    if (count_ >= flush_every_N_)
    {
        count_ = 0;
        ::fflush(fp_);
    }
}

void LogFile::Flush()
{
    ::fflush(fp_);
}
