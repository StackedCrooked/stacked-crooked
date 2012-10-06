#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cxxabi.h>
#include <stdint.h>
#include <sys/time.h>
#include <typeinfo>


std::string demangle(const char * name)
{
    int st;
    char * const p = abi::__cxa_demangle(name, 0, 0, &st);

    if (st != 0)
    {
        switch (st)
        {
            case -1: throw std::runtime_error("A memory allocation failure occurred.");
            case -2: throw std::runtime_error("Not a valid name under the GCC C++ ABI mangling rules.");
            case -3: throw std::runtime_error("One of the arguments is invalid.");
            default: throw std::runtime_error("Unexpected demangle status");
        }
    }

    std::string result(p);
    free(p);
    return result;
}


unsigned counter = 0;
const unsigned iterations = 100000;


template<typename T>
struct Header { T a, b, c; };


double get_current_time()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return double (tv.tv_sec) + 0.000001 * tv.tv_usec;
}


template<typename T>
std::vector<uint8_t> get_buffer()
{
    std::vector<uint8_t> buffer(sizeof(long) + sizeof(Header<T>) * iterations);
    for (std::vector<uint8_t>::size_type i = 0; i != buffer.size(); ++i)
    {
        buffer[i] = buffer.size();
    }
    std::random_shuffle(buffer.begin(), buffer.end());
    return buffer;
}


template<typename T>
void run_benchmark(const uint8_t * data)
{
    for (unsigned i = 0; i != iterations; ++i)
    {
        const Header<T> * hdr = reinterpret_cast<const Header<T>*>(data);
        counter += hdr[i].b;
    }
}


template<typename T>
void run()
{
    std::cout << "\n*** " << demangle(typeid(T).name()) << " ***" << std::endl;
    for (unsigned offset = 0; offset <= (sizeof(T) / 2); ++offset)
    {
        static std::vector<uint8_t> buffer = get_buffer<T>();
        auto start = get_current_time();
        run_benchmark<T>(buffer.data() + offset);
        auto elapsed = get_current_time() - start;
        std::cout << "offset " << offset << ": " << int(0.5 + 1000.0 * 1000.0 * elapsed) << " microseconds" << std::endl;
    }
}


int main()
{
    run<uint16_t>();
    run<uint32_t>();
    run<uint64_t>();
}
