#include <mutex>
#include <chrono>
#include <iostream>
#include <thread>


std::mutex mutex;


int main()
{
    enum { num_iterations = 50 * 1000 * 1000 };
    
    using Clock = std::chrono::steady_clock;
    auto start_time = Clock::now();

    std::thread([] {
        for (auto i = 0ul; i != num_iterations; ++i) {
            mutex.lock();
            mutex.unlock();
        }
    }).join();

    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start_time).count();

    std::cout
        << " lock+unlock=" << int(0.5 + 1.0 * elapsed_ns / num_iterations) << "ns"
		<< std::endl;
}
