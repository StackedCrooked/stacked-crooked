#pragma once


#include <sstream>


// Thread-safe printing to std::cout
// Usage example: Log() << "message";
struct Log : std::ostringstream
{
    Log() = default;
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    ~Log();
};
