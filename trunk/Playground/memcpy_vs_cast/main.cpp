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
    return *reinterpret_cast<const int*>(data);
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

template<typename Function>
unsigned benchmark(const Function & f, std::vector<int> & container, unsigned & counter)
{
    enum { iterations = 1 * 1000 * 1000 };
    double start = get_current_time();
    for (unsigned i = 0; i != iterations; ++i)
    {
        const char * binary = reinterpret_cast<const char *>(container.data());
        unsigned offset = sizeof(int) * (random() % container.size());
        counter += f(binary + offset);
    }
    return unsigned(0.5 + 1000 * (get_current_time() - start));
}


int main()
{
    srand(time(0));
    unsigned counter = 0;

    // Repeat benchmark 4 times.
    for (unsigned i = 0; i != 4; ++i)
    {
        
        std::vector<int> numbers(1000);
        for (std::vector<int>::size_type i = 0; i != numbers.size(); ++i)
        {
            numbers[i] = rand();
        }
        
        std::cout << std::setprecision(5);
        std::cout << "cast      " << benchmark(&test_cast, numbers, counter)     << "ms" << std::endl;
        std::cout << "memcpy    " << benchmark(&test_memcpy, numbers, counter)   << "ms" << std::endl;
        std::cout << "std::copy " << benchmark(&test_std_copy, numbers, counter) << "ms" << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "\n(Optimization prevention counter: " << counter << ")" << std::endl;
}

