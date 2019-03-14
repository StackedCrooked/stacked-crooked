#include <iostream>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <thread>


using Clock = std::chrono::system_clock;
using Milliseconds = std::chrono::milliseconds;
using Nanoseconds = std::chrono::nanoseconds;


extern volatile const int64_t volatile_zero;
extern volatile int64_t volatile_sink;


int64_t get_rdtsc()
{
    unsigned hi, lo;
    asm volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return (uint64_t(hi) << 32) | lo;
}


double detect_cpu_ghz_impl(Milliseconds ms)
{
    auto start_tsc = get_rdtsc();
    auto start_time = Clock::now();

    std::this_thread::sleep_for(ms);

    auto end_time = Clock::now();
    auto elapsed_tsc = get_rdtsc() - start_tsc;
    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();

    return 1.0 * elapsed_tsc / elapsed_ns;
}



static const double ghz = detect_cpu_ghz_impl(Milliseconds(200));


int64_t now()
{
    return get_rdtsc();
}


enum { repetitions = 1024 };


uint64_t* allocate_items(int64_t n);




template<int64_t modulo>
inline int64_t benchmark(int64_t n, uint64_t& sum)
{
    auto items = allocate_items(n);
    for (auto i = 0; i != n; ++i)
    {
        items[i] = i + n * volatile_zero;
    }

    auto start_time = now();

    assert(n % 4 == 0); // required for unrolling
    for (auto i = 0; i != repetitions; ++i)
    {
        auto b = items;
        auto e = b + n;
        for (; b != e; b += 4)
        {
            sum += b[0] % modulo;
            sum += b[1] % modulo;
            sum += b[2] % modulo;
            sum += b[3] % modulo;
        }
    }

    return now() - start_time;
}


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << argv[0] << " NumberOfItems" << std::endl;
        return 1;
    }

    std::cout << "GHz=" << ghz << std::endl;

    int64_t num_items = std::strtoul(argv[1], nullptr, 10);

    { static constexpr int64_t modulo = 4; uint64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 8; uint64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 16; uint64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 32; uint64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 64; uint64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 128; uint64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 10; uint64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 100; uint64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 1000; uint64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 10000; uint64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 100000; uint64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 1000; int64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
    { static constexpr int64_t modulo = 1e9; int64_t sum = 0; auto cycles = benchmark<modulo>(num_items, sum); std::cout << "sum=" << sum << " num_items=" << num_items << " modulo=" << modulo << " cycles=" << cycles << " cycles_per_operation=" << (1.0 * cycles / num_items / repetitions) << std::endl; }
}
