#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iomanip> 
#include <iostream> 
#include <vector>
#include <sys/time.h>

double get_current_time()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return double (tv.tv_sec) + 0.000001 * tv.tv_usec;
}

int test_cast(const char * data)
{
    return *((int*)data);
}

int test_memcpy(const char * data)
{
    int result;
    memcpy(&result, data, sizeof(result));
    return result;
}

int test_std_copy(const char * data)
{
    int result;
    std::copy(data, data + sizeof(int), reinterpret_cast<char *>(&result));
    return result;
}

std::vector<int> get_random_numbers()
{
    std::vector<int> numbers;
    numbers.reserve(1000);
    for (std::vector<int>::size_type i = 0; i != numbers.capacity(); ++i)
    {
        numbers.push_back(rand());
    }
    return numbers;
}

template<typename Function>
unsigned benchmark(const Function & f, unsigned & counter)
{
    enum { iterations = 1 * 1000 * 1000 };
    std::vector<int> container = get_random_numbers();
    double start = get_current_time();
    for (unsigned i = 0; i != iterations; ++i)
    {
        const char * binary_data = reinterpret_cast<const char*>(container.data());
        unsigned random_offset = sizeof(int) * (random() % container.size()); // aligned to int
        counter += f(binary_data + random_offset);
    }
    return unsigned(0.5 + 1000.0 * (get_current_time() - start));
}

int main()
{
    srand(time(0));
    unsigned counter = 0;

    std::cout << "cast:      " << benchmark(&test_cast,     counter)     << " ms" << std::endl;
    std::cout << "memcpy:    " << benchmark(&test_memcpy,   counter)   << " ms" << std::endl;
    std::cout << "std::copy: " << benchmark(&test_std_copy, counter) << " ms" << std::endl;
    std::cout << "(counter:  " << counter << ")" << std::endl << std::endl;
}

