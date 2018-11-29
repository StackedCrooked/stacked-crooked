#include <iostream>
#include <array>
#include <thread>
#include <chrono>


std::ostream& operator<<(std::ostream& os, std::chrono::nanoseconds ns)
{
	return os << ns.count() << "ns";
}





static const volatile auto volatile_zero = 0;
static volatile auto volatile_sink = 0;

std::array<int64_t, 1024 * 1024> items __attribute__((aligned(64)));




int64_t test_increment(int64_t skip)
{
	int64_t result = 0;
	for (auto i = 0ul; i < items.size(); i += skip)
	{
		result += ++items[i];
	}
	return result;
}


int64_t test_write(int64_t skip)
{
	auto value = volatile_zero;
	for (auto i = 0ul; i < items.size(); i += skip)
	{
		items[i] = value;
	}
	return 0;
}


int64_t test_read(int64_t skip)
{
	int64_t sum = 0;
	for (auto i = 0ul; i < items.size(); i += skip)
	{
		sum += items[i];
	}
	volatile_sink = sum;
	return sum;
}


int64_t test_memset(int64_t)
{
	memset(items.data(), 0, items.size());
	return items[volatile_zero] + volatile_zero;
}


using Clock = std::chrono::system_clock;


template<typename F>
std::chrono::nanoseconds benchmark_one(F&& f, int skip)
{
	items[volatile_zero] = volatile_zero;
	auto start_time = Clock::now();
	auto r = f(skip);
	auto result = Clock::now() - start_time;
	volatile_sink = r;
	return result;
}

template<typename F>
std::chrono::nanoseconds benchmark(const char* name, F&& f, int skip)
{
	std::cout << name << ":skip=" << skip << ": " << std::flush;
	std::array<std::chrono::nanoseconds, 8> results;
	for (auto& result : results)
	{
		result = benchmark_one(f, skip);
	}
	std::sort(results.begin(), results.end());
	auto result = results.front();
	std::cout << result << " result*skip=" << result.count() * skip << "\n";
	return result;
}


#define BENCHMARK(name, skip) benchmark(#name, name, skip)
int main()
{
	BENCHMARK(test_increment, 1);
	BENCHMARK(test_increment, 2);
	BENCHMARK(test_increment, 3);
	BENCHMARK(test_increment, 4);
	BENCHMARK(test_increment, 5);
	BENCHMARK(test_increment, 8);
	BENCHMARK(test_increment, 16);
	BENCHMARK(test_increment, 32);
	BENCHMARK(test_increment, 64);
	BENCHMARK(test_increment, 128);
	BENCHMARK(test_increment, 256);
	BENCHMARK(test_increment, 512);
	BENCHMARK(test_increment, 1024);

	BENCHMARK(test_read, 1);
	BENCHMARK(test_read, 2);
	BENCHMARK(test_read, 3);
	BENCHMARK(test_read, 4);
	BENCHMARK(test_read, 5);
	BENCHMARK(test_read, 8);
	BENCHMARK(test_read, 16);
	BENCHMARK(test_read, 32);
	BENCHMARK(test_read, 64);
	BENCHMARK(test_read, 128);
	BENCHMARK(test_read, 256);
	BENCHMARK(test_read, 512);
	BENCHMARK(test_read, 1024);

	BENCHMARK(test_write, 1);
	BENCHMARK(test_write, 2);
	BENCHMARK(test_write, 3);
	BENCHMARK(test_write, 4);
	BENCHMARK(test_write, 5);
	BENCHMARK(test_write, 8);
	BENCHMARK(test_write, 16);
	BENCHMARK(test_write, 32);
	BENCHMARK(test_write, 64);
	BENCHMARK(test_write, 128);
	BENCHMARK(test_write, 256);
	BENCHMARK(test_write, 512);
	BENCHMARK(test_write, 1024);
	BENCHMARK(test_memset, 1024);

}
