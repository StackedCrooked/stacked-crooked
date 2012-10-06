#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <stdint.h>
#include <sys/time.h>

struct NetworkData
{
    uint64_t a, b, c;
};

double GetCurrentTime()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return double (tv.tv_sec) + 0.000001 * tv.tv_usec;
}

uint8_t GetRandomByte()
{
    static auto rng = []() -> std::mt19937 {
        std::mt19937 rng;
        rng.seed(time(0));
        return rng;
    }();
    static std::uniform_int_distribution<uint8_t> dist(0, 255);
    return dist(rng);
}


std::size_t counter = 0; // prevent GCC from optimizating the entire program away


struct copier_stdcopy
{
    static const char * name() { return "stdcopy"; }

    void operator()(uint8_t * src) const
    {
        NetworkData hdr;
        uint8_t * dst = reinterpret_cast<uint8_t*>(&hdr);
        std::copy(src, src + sizeof(hdr), dst);
        counter += hdr.b;
    }

};

struct copier_memcpy
{
    static const char * name() { return "memcpy"; }

    void operator()(uint8_t * src) const
    {
        NetworkData hdr;
        memcpy(&hdr, src, sizeof(hdr));
        counter += hdr.b;
    }

};

struct copier_assign
{
    static const char * name() { return "assign"; }

    void operator()(uint8_t * src) const
    {
        NetworkData hdr = *reinterpret_cast<NetworkData*>(src);
        counter += hdr.b;
    }

};


const std::size_t iterations = 1000000;
const uint64_t millisecond = uint64_t(1000);
const uint64_t microseconds = uint64_t(1000) * uint64_t(1000);
const uint64_t nanosecond = uint64_t(1000) * microseconds;


std::vector<uint8_t> GetBuffer()
{
    std::vector<uint8_t> buffer(1 + iterations * sizeof(NetworkData));
    for (auto & c : buffer)
    {
        c = GetRandomByte();
    }
    return buffer;
}


enum Alignment
{
    Alignment_OK,
    Alignment_NOK
};


template<typename CopyFunction>
void test(const CopyFunction & inCopyFunction, Alignment inAlignment)
{
    // Generate buffer large buffer
    std::vector<uint8_t> buffer = GetBuffer();

    auto startTime = GetCurrentTime();
    for (std::size_t i = 0; i != iterations; ++i)
    {
        inCopyFunction(&buffer[(i * sizeof(NetworkData)) + (inAlignment == Alignment_OK ? 0 : 1)]);
    }
    auto elapsed = GetCurrentTime() - startTime;
    std::cout << CopyFunction::name() << " with: " << (millisecond * elapsed) << " millisecond" << std::endl;
}


int main()
{
    std::setprecision(0);
    std::cout.setf(std::ios::fixed);


    std::cout << "\nTest with good alignment" << std::endl;
    test(copier_stdcopy(), Alignment_OK);
    test(copier_memcpy(), Alignment_OK);
    test(copier_assign(), Alignment_OK);

    std::cout << "\nTest with bad alignment" << std::endl;
    test(copier_stdcopy(), Alignment_NOK);
    test(copier_memcpy(), Alignment_NOK);
    test(copier_assign(), Alignment_NOK);
    std::cout << "optimization prevention counter: " << counter << std::endl;

}
