#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cxxabi.h>
#include <stdint.h>
#include <sys/time.h>
#include <typeinfo>


struct u128 { uint64_t a, b; };
struct u256 { u128 a, b; };
struct u512 { u256 a, b; };
struct u1024 { u512 a, b; };


struct counter_t
{
    counter_t() : value() {}

    operator uint64_t() const { return value; }

    void operator+=(uint16_t v) { value = v; }
    void operator+=(uint32_t v) { value = v; }
    void operator+=(uint64_t v) { value = v; }
    void operator+=(u128 v)     { value = v.a; }
    void operator+=(u256 v)     { value = v.b.a; }
    void operator+=(u512 v)     { value = v.a.b.a; }
    void operator+=(u1024 v)     { value = v.a.b.a.b; }


    uint64_t value;
};


counter_t counter;


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
void run_benchmark(const uint8_t * data, const std::vector<std::size_t> & indices)
{
    for (unsigned i = 0; i != iterations; ++i)
    {
        const Header<T> * hdr = reinterpret_cast<const Header<T>*>(data);
        counter += hdr[indices[i]].b;
    }
}


template<typename T>
void run()
{
    std::cout << "\n*** " << demangle(typeid(T).name()) << " ***" << std::endl;
    for (unsigned offset = 0; offset < (4 * sizeof(T)); ++offset)
    {
        static std::vector<uint8_t> buffer = get_buffer<T>();

        std::vector<std::size_t> indices;
        while (indices.size() < iterations) indices.push_back(indices.size());
        std::random_shuffle(indices.begin(), indices.end());

        auto start = get_current_time();
        run_benchmark<T>(buffer.data() + offset, indices);
        auto elapsed = get_current_time() - start;
        std::cout << "offset " << std::setw(4) << offset << ": " << std::string(unsigned(40000 * elapsed), '#') << std::endl;
    }
}


int main()
{
    run<uint16_t>();
    run<uint32_t>();
    run<uint64_t>();
    run<u128>();
//    run<u256>();
//    run<u512>();
//    run<u1024>();
    std::cout << "counter: " << counter << std::endl;
}
