#include <algorithm>
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

struct test_cast
{
    int operator()(const char * data) const 
    {
        return *((int*)data);
    }
};

struct test_memcpy
{
    int operator()(const char * data) const 
    {
        int result;
        memcpy(&result, data, sizeof(result));
        return result;
    }
};

struct test_std_copy
{
    int operator()(const char * data) const 
    {
        int result;
        std::copy(data, data + sizeof(int), reinterpret_cast<char *>(&result));
        return result;
    }
};

enum
{
    iterations = 2000,
    container_size = 2000
};

std::vector<int> get_random_numbers()
{
    std::vector<int> numbers(container_size);
    for (std::vector<int>::size_type i = 0; i != numbers.size(); ++i)
    {
        numbers[i] = rand();
    }
    return numbers;
}

std::vector<int> get_random_indices()
{
    std::vector<int> numbers(container_size);
    for (std::vector<int>::size_type i = 0; i != numbers.size(); ++i)
    {
        numbers[i] = i;
    }
    std::random_shuffle(numbers.begin(), numbers.end());
    return numbers;
}

template<typename Function>
unsigned benchmark(const Function & f, unsigned & counter)
{
    std::vector<int> container = get_random_numbers();
    std::vector<int> indices = get_random_indices();
    double start = get_current_time();
    for (unsigned iter = 0; iter != iterations; ++iter)
    {
        for (unsigned i = 0; i != container.size(); ++i)
        {
            counter += f(reinterpret_cast<const char*>(&container[indices[i]]));
        }
    }
    return unsigned(0.5 + 1000.0 * (get_current_time() - start));
}

int main()
{
    srand(time(0));
    unsigned counter = 0;

    std::cout << "cast:      " << benchmark(test_cast(),     counter) << " ms" << std::endl;
    std::cout << "memcpy:    " << benchmark(test_memcpy(),   counter) << " ms" << std::endl;
    std::cout << "std::copy: " << benchmark(test_std_copy(), counter) << " ms" << std::endl;
    std::cout << "(counter:  " << counter << ")" << std::endl << std::endl;

}

