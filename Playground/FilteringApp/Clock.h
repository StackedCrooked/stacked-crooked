#pragma once


#include <chrono>


using Clock = std::chrono::steady_clock;


// To avoid reording we need to use RDTSCP for stopping a benchmark while RDTSC may be used for starting a benchmark.
// http://blog.regehr.org/archives/330
struct Benchmark
{
    using duration = int64_t;

    static uint64_t start()
    {
        uint32_t lo, hi;

        asm volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (hi), "=r" (lo)::
            "%rax", "%rbx", "%rcx", "%rdx");

        return (uint64_t(hi) << 32) | uint64_t(lo);
    }

    // stop requires the RDTSCP instruction
    static uint64_t stop()
    {
        uint32_t lo, hi;

        asm volatile(
            "RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (hi), "=r" (lo):: "%rax",
            "%rbx", "%rcx", "%rdx");

        return (uint64_t(hi) << 32) | uint64_t(lo);
    }
};


inline int64_t detect_cpu_hz()
{
    auto start_tsc = Benchmark::start();
    auto start_time = Clock::now();
    auto now = start_time;
    while (now < start_time + std::chrono::milliseconds(200))
    {
        now = Clock::now();
    }
    auto elapsed_tsc = Benchmark::stop() - start_tsc;
    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time).count();

    return 1000000000L * elapsed_tsc / elapsed_ns;
}


static auto cpu_hz = detect_cpu_hz();


inline int64_t rdtsc_elapsed_cycles(int64_t rdtsc_start_time)
{
    return Benchmark::stop() - rdtsc_start_time;
}



inline int64_t rdtsc_elapsed_ns(int64_t rdtsc_start_time)
{
    return 1e9 * rdtsc_elapsed_cycles(rdtsc_start_time) / cpu_hz;
}
