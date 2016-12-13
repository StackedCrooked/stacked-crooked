#include <cstdint>
#include <numeric>
#include <vector>
#include <thread>
#include <iostream>

using Integer = uint32_t;


std::vector<Integer> GenerateVector(std::size_t size)
{
    std::vector<Integer> result;
    result.reserve(size);
    while (result.size() != result.capacity())
    {
        result.push_back(result.size());
    }
    return result;
}


auto vec_512 = GenerateVector(512);
auto vec_1024 = GenerateVector(1024);
auto vec_2048 = GenerateVector(2048);
auto vec_4096 = GenerateVector(4096);
auto vec_8192 = GenerateVector(8192);
auto vec_16384 = GenerateVector(16384);
auto vec_32768 = GenerateVector(32768);
auto vec_134217728 = GenerateVector(134217728);



Integer sum(std::vector<Integer>& vec)
{
    return std::accumulate(vec.begin(), vec.end(), 0u, [](Integer a, Integer b) { return a + b; });
}


volatile Integer volatile_sink;


template<typename F>
void benchmark(F&& f)
{
    using Clock = std::chrono::steady_clock;
    auto t1 = Clock::now();
    volatile_sink = f();
    auto t2 = Clock::now();
    volatile_sink = f();
    auto t3 = Clock::now();
    volatile_sink = f();
    auto t4 = Clock::now();
    std::cout
        << "  " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() << '\n'
        << "  " << std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t2).count() << '\n'
        << "  " << std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count() << "\n\n";
}

int main()
{
#define B(n) std::cout << #n << ": " << std::endl; benchmark([]{ return sum(vec_##n); });
    B(134217728)
    B(32768)
    B(8192)
    B(4096)
    B(2048)
    B(1024)
}
