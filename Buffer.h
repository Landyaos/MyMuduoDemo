#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>


#include <vector>

class Buffer
{
public:
    Buffer();
    ~Buffer();
    




private:
    const size_t kInitialSize = 1024;

    std::vector<char> buffer_;
    size_t read_idx_;
    size_t wirte_idx_;

    size_t read_available_;
    size_t write_available_;
};

#endif