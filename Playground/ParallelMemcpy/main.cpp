#include <atomic>
#include <array>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <chrono>


__attribute__((aligned(128))) std::atomic<long> control_flag{0};
__attribute__((aligned(128))) const volatile long volatile_zero = 0;
__attribute__((aligned(128))) volatile long volatile_sink = 0;


enum { memory_size = 1024UL * 1024UL }; // Coliru doesn't have much


auto make_memory = []{
    auto result = static_cast<char*>(malloc(memory_size));
    memset(result, volatile_zero, memory_size);
    return result;
};



std::array<const char*, 8> sources = {{
    make_memory(), make_memory(), make_memory(), make_memory(),
    make_memory(), make_memory(), make_memory(), make_memory()
}};

std::array<char*, 8> targets = {{
    make_memory(), make_memory(), make_memory(), make_memory(),
    make_memory(), make_memory(), make_memory(), make_memory()
}};



void start_thread(int thread_index)
{
    // signal main thread that we have started
    control_flag++;

    // wait for main thread to signal start
    while (control_flag > 0) { asm volatile ("pause;"); }

    memcpy(targets[thread_index], sources[thread_index], memory_size);

    volatile_sink = targets[thread_index][volatile_zero];
    
    // signal main thread that we have finished
    control_flag++;
}


void run(long num_threads)
{
    control_flag = 0;

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (auto i = 0; i != num_threads; ++i)
    {
        threads.emplace_back(start_thread, i);
        threads.back().detach();
    }


    // wait until all threads have launched
    while (control_flag != num_threads) { asm volatile ("pause;"); }

    // signal the threads to start
    control_flag = 0;


    auto start_time = std::chrono::high_resolution_clock::now();

    // wait until threads have finished
    while (control_flag != num_threads) { asm volatile ("pause;"); }

    auto elapsed_ns = 1.0 * std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start_time).count();

    auto total_size = memory_size * num_threads;

    std::cout
        << "size: " << memory_size
        << ". total size: " << num_threads * memory_size
        << ". num-threads: " << num_threads
        << ". elapsed_ns: " << long(0.5 + elapsed_ns)
        << ". gbits_per_s: " << long(0.5 + 8.0 * total_size / elapsed_ns)
        << std::endl;
}


int main()
{
    for (auto i = 1; i != sources.size(); ++i)
    {
        run(i);
    }
}

