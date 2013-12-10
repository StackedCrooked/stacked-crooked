#include <string.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>


using namespace std::chrono;
using Clock = std::chrono::high_resolution_clock;


long size = 1500;
long iterations = 1000 * 1000;





void* get(long size)
{
	static std::vector<void*> buf;
	if (buf.size() < 1000)
	{
		buf.push_back(malloc(size));
		return buf.back();
	}
	static unsigned i = 0;
	return buf[i++ % buf.size()];
}


void* dst(long size) { return get(size); }
void* src(long size) { return get(size); }






int main()
{
    
    auto start = Clock::now();

    for (long i = 0; i != iterations; ++i)
    {
		memcpy(dst(size), src(size), size);
    }

    auto us = duration_cast<microseconds>(Clock::now() - start).count();
    auto megabyte_per_second = iterations * size / us;

    std::cout << megabyte_per_second << "MB/s" << std::endl;
}
