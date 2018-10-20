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

#include <algorithm>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <array>


typedef uint32_t xmm32_t __attribute__ ((vector_size (16))) __attribute__((aligned(16)));
typedef uint64_t xmm64_t __attribute__ ((vector_size (16))) __attribute__((aligned(16)));
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


inline void xmm32_generate_random_data(xmm32_t* dst, uint64_t len)
{
    struct Seeds
    {
        xmm32_t a = {1, 3, 5, 7};
        xmm32_t b = {11, 13, 17, 19};
        xmm32_t c = {23, 29, 31, 37};
        xmm32_t d = {43, 47, 53, 57};
    };

    Seeds seeds;
    xmm32_t multipliers = { 1103515245, 1103515245, 1103515245, 1103515245 };
    xmm32_t increments = { 11, 11, 11, 11 };

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


inline void xmm64_generate_random_data(xmm64_t* dst, uint64_t len)
{
    struct Seeds
    {
        xmm64_t a = {1, 3 };
        xmm64_t b = {11, 13 };
        xmm64_t c = {23, 29 };
        xmm64_t d = {43, 47 };
    };

    Seeds seeds;
    xmm64_t multipliers = { 1103515245, 1103515245 };
    xmm64_t increments = { 11, 11 };

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
        seeds.c = seeds.c * multipliers[2] + increments[2];
        seeds.d = seeds.d * multipliers[3] + increments[3];

        *dst++ = seeds.a;
        *dst++ = seeds.b;
        *dst++ = seeds.c;
        *dst++ = seeds.d;
        len -= sizeof(seeds);
    }
}


inline void u32_generate_random_data(uint32_t* dst, uint64_t len)
{
    struct Seeds
    {
        uint32_t a = 1;
        uint32_t b = 3;
        uint32_t c = 11;
        uint32_t d = 13;
    };

    Seeds seeds;
    uint32_t multipliers[4] = { 1103515245, 1103515245, 1103515245, 1103515245 };
    uint32_t increments[4] = { 11, 11, 11, 11, };

    while (len >= sizeof(seeds))
    {
        seeds.a = seeds.a * multipliers[0] + increments[0];
        seeds.b = seeds.b * multipliers[1] + increments[1];
        seeds.c = seeds.c * multipliers[2] + increments[2];
        seeds.d = seeds.d * multipliers[3] + increments[3];

        *dst++ = seeds.a;
        *dst++ = seeds.b;
        *dst++ = seeds.c;
        *dst++ = seeds.d;
        len -= sizeof(seeds);
    }
}



enum : uint64_t
{
    num_ymm = 2 * 1024UL * 1024UL * 1024UL / sizeof(ymm_t), // don't use too much memory on Coliru
    num_xmm = 2 * num_ymm,
    num_u64 = 2 * num_xmm,
    num_u32 = 2 * num_u64
};


union Buffer
{
    ymm_t    ymm[num_ymm];
    xmm32_t  xmm32[num_xmm];
    xmm64_t  xmm64[num_xmm];
    uint64_t u64[num_u64];
    uint32_t u32[num_u32];
};


Buffer buffer;


static_assert(sizeof(buffer.u32  ) == sizeof(buffer.u64  ), "");
static_assert(sizeof(buffer.u64  ) == sizeof(buffer.xmm32), "");
static_assert(sizeof(buffer.xmm32) == sizeof(buffer.xmm64), "");
static_assert(sizeof(buffer.xmm64) == sizeof(buffer.ymm  ), "");


} // namespace 


int main()
{
    for (auto& u64 : buffer.u64)
    {
        u64 = volatile_zero; // disable optimizations
    }

    {
        std::array<std::uint64_t, 8> u32_results;
        for (auto& result : u32_results)
        {
            result = benchmark([&]{ u32_generate_random_data(buffer.u32, sizeof(buffer.u32)/sizeof(buffer.u32[0])); });
            //std::ofstream of("u64"); of.write((char*)buffer.u64, sizeof(buffer));
        }

        std::cout << "u32_results: ";
        for (auto& result : u32_results)
        {
            std::cout << result << ' ';
        }
        std::cout << "min: " << *std::min_element(u32_results.begin(), u32_results.end()) << std::endl;
    }

    {
        std::array<std::uint64_t, 8> u64_results;
        for (auto& result : u64_results)
        {
            result = benchmark([&]{ u64_generate_random_data(buffer.u64, sizeof(buffer.u64)/sizeof(buffer.u64[0])); });
            //std::ofstream of("u64"); of.write((char*)buffer.u64, sizeof(buffer));
        }

        std::cout << "u64_results: ";
        for (auto& result : u64_results)
        {
            std::cout << result << ' ';
        }
        std::cout << "min: " << *std::min_element(u64_results.begin(), u64_results.end()) << std::endl;
    }

    {
        std::array<std::uint64_t, 8> xmm32_results;
        for (auto& result : xmm32_results)
        {
            result = benchmark([&]{ xmm32_generate_random_data(buffer.xmm32, sizeof(buffer.xmm32)/sizeof(buffer.xmm32[0])); });
            //std::ofstream of("xmm"); of.write((char*)buffer.xmm, sizeof(buffer));
        }
        std::cout << "xmm32_results: ";
        for (auto& result : xmm32_results)
        {
            std::cout << result << ' ';
        }
        std::cout << "min: " << *std::min_element(xmm32_results.begin(), xmm32_results.end()) << std::endl;
    }

    {
        std::array<std::uint64_t, 8> xmm64_results;
        for (auto& result : xmm64_results)
        {
            result = benchmark([&]{ xmm64_generate_random_data(buffer.xmm64, sizeof(buffer.xmm64)/sizeof(buffer.xmm64[0])); });
            //std::ofstream of("xmm"); of.write((char*)buffer.xmm, sizeof(buffer));
        }
        std::cout << "xmm64_results: ";
        for (auto& result : xmm64_results)
        {
            std::cout << result << ' ';
        }
        std::cout << "min: " << *std::min_element(xmm64_results.begin(), xmm64_results.end()) << std::endl;
    }

    {
        std::array<std::uint64_t, 8> ymm_results;
        for (auto& result : ymm_results)
        {
            result = benchmark([&]{ ymm_generate_random_data(buffer.ymm, sizeof(buffer.ymm)/sizeof(buffer.ymm[0])); });
            //std::ofstream of("ymm"); of.write((char*)buffer.ymm, sizeof(buffer));
        }

        std::cout << "ymm_results: ";
        for (auto& result : ymm_results)
        {
            std::cout << result << ' ';
        }
        std::cout << "min: " << *std::min_element(ymm_results.begin(), ymm_results.end()) << std::endl;

    }

    volatile_sink = buffer.u64[volatile_zero];
}
