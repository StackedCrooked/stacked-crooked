#include <string.h>
#include <stdexcept>
#include <future>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>


using namespace std::chrono;
using Clock = std::chrono::high_resolution_clock;



int main(int argc, char** argv)
{
	if (argc != 2) throw std::runtime_error("Invalid argument count.");
	int thread_count = atoi(argv[1]);
	if (thread_count < 1 && thread_count > 20) throw std::runtime_error(std::string("Invalid thread count: ") + argv[1]);

	std::cout << "Number of threads: " << thread_count << ' ' << std::flush;
    long size = 2048;
    long iterations = 10 * 1000 * 1000;
    
    auto src = malloc(size);
    memset(src, 0, size);


    auto test = [&]() -> long
    {
        auto dst = malloc(size);
        auto start = Clock::now();
        for (long i = 0; i != iterations; ++i)
        {
            memcpy(dst, src, size);
        }
        return duration_cast<microseconds>(Clock::now() - start).count();
    };


    std::vector<std::future<long>> futures;
    for (int i = 0; i != thread_count; ++i)
    {
        futures.push_back(std::async(std::launch::async, test));
    }

    long total_time = 0;
    for (auto& fut : futures) total_time += fut.get();


    auto megabyte_per_second = futures.size() * iterations * size / total_time;

    std::cout << megabyte_per_second << "MB/s" << std::endl;
}
