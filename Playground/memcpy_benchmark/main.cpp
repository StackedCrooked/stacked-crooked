#include <string.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>


using namespace std::chrono;
using Clock = std::chrono::high_resolution_clock;



int main()
{
    long size = 1500;
    long iterations = 1000 * 1000;
    
    auto src = malloc(size);
    memset(src, 0, size);

    auto dst = malloc(size);

    auto start = Clock::now();

    for (long i = 0; i != iterations; ++i)
    {
        memcpy(dst, src, size);
    }

    auto us = duration_cast<microseconds>(Clock::now() - start).count();
    auto megabyte_per_second = iterations * size / us;

    std::cout << megabyte_per_second << "MB/s" << std::endl;
}
