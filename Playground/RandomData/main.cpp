/**
 * Simple test where I generate random data using a linear congruential generator (https://en.wikipedia.org/wiki/Linear_congruential_generator)
 * and write that data to memory.
 * 
 * Basically ymm is 2x faster than xmm which is 2x faster than uint64_t.
 * I did not expect such a big difference in performance.
 * I thought memory would have been the dominant cost. But that didn't seem to happen.
 * 
 * (I hope I didn't make many rookie benchmarking mistakes here.)
 */

#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <array>


typedef uint64_t xmm_t __attribute__ ((vector_size (16))) __attribute__((aligned(16)));
typedef uint64_t ymm_t __attribute__ ((vector_size (32))) __attribute__((aligned(32)));


// optimizer disablers
const volatile uint64_t& volatile_zero = *new uint64_t;
volatile uint64_t& volatile_sink = *new uint64_t;


namespace {


inline uint64_t rdtscp()
{
    uint64_t result = 0;
    __asm__ __volatile__(
        "rdtscp;"
        "shl $32, %%rdx;"
        "or %%rdx, %%rax"
        : "=a"(result)
        :
        : "%rcx", "%rdx");

    return result;
}


template<typename F>
uint64_t benchmark(F&& f)
{
    auto t1 = rdtscp();
    f();
    return rdtscp() - t1;
}


inline void ymm_generate_random_data(ymm_t* dst, uint64_t len)
{
    struct Seeds
    {
        ymm_t a = {1, 3, 5, 7};
        ymm_t b = {11, 13, 17, 19};
        ymm_t c = {23, 29, 31, 37};
        ymm_t d = {43, 47, 53, 57};
    };

    Seeds seeds;
    ymm_t multipliers = { 1103515245, 1103515245, 1103515245, 1103515245 };
    ymm_t increments = { 11, 11, 11, 11 };

    while (len >= sizeof(seeds))
    {
        seeds.a = seeds.a * multipliers + increments;
        seeds.b = seeds.b * multipliers + increments;
        seeds.c = seeds.c * multipliers + increments;
        seeds.d = seeds.d * multipliers + increments;

        *dst++ = seeds.a;
        *dst++ = seeds.b;
        *dst++ = seeds.c;
        *dst++ = seeds.d;
        len -= sizeof(seeds);
    }
}


inline void xmm_generate_random_data(xmm_t* dst, uint64_t len)
{
    struct Seeds
    {
        xmm_t a = {1, 3};
        xmm_t b = {11, 13};
    };

    Seeds seeds;
    xmm_t multipliers = { 1103515245, 1103515245 };
    xmm_t increments = { 11, 11 };

    while (len >= sizeof(seeds))
    {
        seeds.a = seeds.a * multipliers + increments;
        seeds.b = seeds.b * multipliers + increments;

        *dst++ = seeds.a;
        *dst++ = seeds.b;
        len -= sizeof(seeds);
    }
}


inline void u64_generate_random_data(uint64_t* dst, uint64_t len)
{
    struct Seeds
    {
        uint64_t a = 1;
        uint64_t b = 3;
        uint64_t c = 11;
        uint64_t d = 13;
    };

    Seeds seeds;
    uint64_t multipliers[4] = { 1103515245, 1103515245, 1103515245, 1103515245 };
    uint64_t increments[4] = { 11, 11, 11, 11, };

    while (len >= sizeof(seeds))
    {
        seeds.a = seeds.a * multipliers[0] + increments[0];
        seeds.b = seeds.b * multipliers[1] + increments[1];
        seeds.c = seeds.a * multipliers[2] + increments[2];
        seeds.d = seeds.b * multipliers[3] + increments[3];

        *dst++ = seeds.a;
        *dst++ = seeds.b;
        *dst++ = seeds.c;
        *dst++ = seeds.d;
        len -= sizeof(seeds);
    }
}


enum : uint64_t
{
    num_ymm = 1024UL * 1024UL * 1024UL / sizeof(ymm_t),
    num_xmm = 2 * num_ymm,
    num_u64 = 2 * num_xmm
};

union Buffer
{
    std::array<ymm_t   , num_ymm> ymm;
    std::array<xmm_t   , num_xmm> xmm;
    std::array<uint64_t, num_u64> u64;
};

Buffer buffer;


static_assert(sizeof(ymm_t) == 2 * sizeof(xmm_t), "");
static_assert(sizeof(xmm_t) == 2 * sizeof(uint64_t), "");

static_assert(sizeof(buffer.ymm) == sizeof(buffer.xmm), "");
static_assert(sizeof(buffer.xmm) == sizeof(buffer.u64), "");

static_assert(sizeof(buffer.xmm.size()) == sizeof(buffer.ymm.size()), "");
static_assert(sizeof(buffer.u64.size()) == sizeof(buffer.ymm.size()), "");

static_assert(buffer.ymm.size() == buffer.xmm.size() / 2, "");
static_assert(buffer.ymm.size() == buffer.u64.size() / 4, "");
static_assert(buffer.xmm.size() == buffer.u64.size() / 2, "");


}


int main()
{
    for (auto& u64 : buffer.u64)
    {
        u64 = volatile_zero;
    }

    volatile_sink = benchmark([&]{ u64_generate_random_data(buffer.u64.data(), buffer.u64.size()); });
    volatile_sink = benchmark([&]{ xmm_generate_random_data(buffer.xmm.data(), buffer.xmm.size()); });
    volatile_sink = benchmark([&]{ ymm_generate_random_data(buffer.ymm.data(), buffer.ymm.size()); });

    {
        std::array<std::uint64_t, 8> u64_results;
        for (auto& result : u64_results)
        {
            result = benchmark([&]{ u64_generate_random_data(buffer.u64.data(), buffer.u64.size()); });
            //std::ofstream of("u64"); of.write((char*)buffer.u64.data(), sizeof(buffer));
        }

        std::cout << "u64_results: ";
        for (auto& result : u64_results)
        {
            std::cout << result << ' ';
        }
        std::cout << std::endl;
    }

    {
        std::array<std::uint64_t, 8> xmm_results;
        for (auto& result : xmm_results)
        {
            result = benchmark([&]{ xmm_generate_random_data(buffer.xmm.data(), buffer.xmm.size()); });
            //std::ofstream of("xmm"); of.write((char*)buffer.xmm.data(), sizeof(buffer));
        }
        std::cout << "xmm_results: ";
        for (auto& result : xmm_results)
        {
            std::cout << result << ' ';
        }
        std::cout << std::endl;
    }

    {
        std::array<std::uint64_t, 8> ymm_results;
        for (auto& result : ymm_results)
        {
            result = benchmark([&]{ ymm_generate_random_data(buffer.ymm.data(), buffer.ymm.size()); });
            //std::ofstream of("ymm"); of.write((char*)buffer.ymm.data(), sizeof(buffer));
        }

        std::cout << "ymm_results: ";
        for (auto& result : ymm_results)
        {
            std::cout << result << ' ';
        }
        std::cout << std::endl;

    }

    volatile_sink = buffer.u64[volatile_zero];
}
