#include <string.h>
#include <iostream>
#include <chrono>
#include <vector>


using namespace std::chrono;
using Clock = std::chrono::high_resolution_clock;



int main()
{
    long size = 1500;
    long iterations = 1000 * 1000;
    
    auto src = malloc(size);
    auto dst = malloc(size);

    memset(dst, 0, size);

    auto start = Clock::now();
    for (long i = 0; i != iterations; ++i)
    {
        memcpy(dst, src, size);
    }    

    auto megabyte_per_second = iterations * size / duration_cast<microseconds>(Clock::now() - start).count();

    std::cout << megabyte_per_second << "MB/s" << std::endl;
}
