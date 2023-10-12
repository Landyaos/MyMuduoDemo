#include "Timestamp.h"

Timestamp::Timestamp()
    : time_point_(std::chrono::high_resolution_clock::now())
{
}

Timestamp::Timestamp(long long duration)
    : time_point_(std::chrono::high_resolution_clock::now() + std::chrono::microseconds(duration))
{
}
