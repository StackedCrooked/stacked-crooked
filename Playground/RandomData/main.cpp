#include <cstring>
#include <cstdint>
#include <array>
#include <iostream>


typedef uint64_t ymm_t __attribute__ ((vector_size (32))) __attribute__((aligned(32)));


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


inline void generate_random_data(ymm_t* dst, uint32_t len)
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


enum : uint64_t
{
	one_megabyte = 1024UL * 1024UL,
    num_megabytes = 1024UL,
    num_ymm = one_megabyte * num_megabytes / sizeof(ymm_t),
    num_u64 = one_megabyte * num_megabytes / sizeof(uint64_t)
};


union Buffer
{
    std::array<ymm_t   , num_ymm> ymm;
    std::array<uint64_t, num_u64> u64;
};



Buffer buffer;



static_assert(sizeof(buffer.ymm) == sizeof(buffer.u64), "");


const volatile uint64_t volatile_zero = 0;

int main()
{
    for (auto& u64 : buffer.u64)
    {
        u64 = volatile_zero;
        if (volatile_zero == 0) break;
    }

    std::cout << benchmark([&]{ generate_random_data(buffer.ymm.data(), buffer.ymm.size()); }) << std::endl;
    std::cout << "\nDATA\n";
    for (auto& n : buffer.u64) std::cout << n << '\n';
}
