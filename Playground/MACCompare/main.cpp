#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <unordered_map>
#include <sys/time.h>


double GetCurrentTime()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return double (tv.tv_sec) + 0.000001 * tv.tv_usec;
}


//! Returns a vector containing range [0, 256).
//! The vector shuffled before returning. This makes
//! it some sort of random number generator.
static const std::vector<std::uint8_t> & GetRandomNumbers()
{
    static std::vector<std::uint8_t> fNumbers = [](){
        std::vector<std::uint8_t> numbers;
        for (unsigned i = 0; i != 255; ++i) { numbers.push_back(i%4); }
        return numbers;
    }();
    std::random_shuffle(fNumbers.begin(), fNumbers.end());
    return fNumbers;
}


//! 6-byte field for emulating MAC address
typedef std::array<uint8_t, 6> MAC;


//! Returns a random MAC address.
MAC GetRandomMAC()
{
    MAC result;
    const std::vector<std::uint8_t> & numbers = GetRandomNumbers();
    result[0] = numbers[0];
    result[1] = numbers[1];
    result[2] = numbers[2];
    result[3] = numbers[3];
    result[4] = numbers[3];
    result[5] = numbers[4];
    return result;
}


typedef std::map<MAC, bool> Map;


struct Hash
{
    template<typename MAC>
    std::size_t operator()(const MAC & mac) const
    {
        static_assert(sizeof(std::size_t) >= 6, "MAC address doesn't fit in std::size_t!");
        std::size_t key = 0;
        key |= size_t(mac[0]) << 56;
        key |= size_t(mac[1]) << 48;
        key |= size_t(mac[2]) << 40;
        key |= size_t(mac[3]) << 32;
        key |= size_t(mac[4]) << 24;
        key |= size_t(mac[5]) << 16;
        return key;
    }
};



typedef std::unordered_map<MAC, bool, Hash> HashMap;


std::size_t cSize = 1;


HashMap gHashMap = [](std::size_t inSize) {
    HashMap result;
    while (result.size() < inSize)
    {
        result.insert(std::make_pair(GetRandomMAC(), false));
    }
    return result;
}(cSize);





template<typename ContainerType>
std::tuple<unsigned, unsigned, unsigned> Benchmark(const ContainerType & container)
{

    // Generate a list of random MAC addresses
    std::size_t cNumIterations = 100000;
    static std::vector<MAC> randomMacs = [=]() {
        std::vector<MAC> macs;
        for (std::size_t idx = 0; idx != cNumIterations; ++idx) {
            macs.push_back(GetRandomMAC());
        }
        return macs;
    }();

    // Search each random mac in the container and count the results.
    // (The counter was introduced to prevent the compiler from optimizing everything away.)
    double beginTime = GetCurrentTime();
    unsigned found = 0;
    for (auto & mac : randomMacs)
    {
        if (container.find(mac) != container.end())
        {
            found++;
        }
    }

    return std::tuple<unsigned, unsigned, unsigned>(unsigned(0.5 + 1000 * (GetCurrentTime() - beginTime)),
                                                    found,
                                                    cNumIterations);
}


std::ostream & operator<<(std::ostream & os, const std::tuple<unsigned, unsigned, unsigned> & result)
{
    os << std::right << std::setw(10) << (std::to_string(std::get<0>(result)) + " ms") << std::flush;
    os << std::left << std::setw(20) << std::string(" (" + std::to_string(std::get<1>(result)) + "/" + std::to_string(std::get<2>(result)) + ").");
    return os;
}


int main()
{
    auto GetMap = [](std::size_t inSize) {
            Map result;
            while (result.size() < inSize)
            {
                result.insert(std::make_pair(GetRandomMAC(), false));
            }
            return result;
        };

    auto GetHashMap = [](std::size_t inSize) {
        HashMap result;
        while (result.size() < inSize)
        {
            result.insert(std::make_pair(GetRandomMAC(), false));
        }
        return result;
    };


    std::cout << "size, map/hash" << std::endl;

    for (unsigned i = 1; i <= 1024; i *= 2)
    {
        std::cout << std::right << std::setw(5) << std::setfill(' ') << i << std::flush
                  << std::right << Benchmark(GetMap(i)) << std::flush
                  << std::right << Benchmark(GetHashMap(i)) << std::endl;
    }
}
