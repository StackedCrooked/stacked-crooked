#include <boost/functional/hash.hpp>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <iomanip>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <sys/time.h>


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


//! 6-byte field for emulating MAC address
typedef std::array<uint8_t, 6> MAC;


typedef std::map<MAC, bool> Map;


struct Hash {
    std::size_t operator()(const std::array<uint8_t, 6> & mac) const {
        static_assert(sizeof(std::size_t) >= 6, "MAC address doesn't fit in std::size_t!");
        std::size_t key = 0;

        // Possibly UB
        boost::hash_combine(key, 0x0000FFFFFFFFFFFF & reinterpret_cast<const uint64_t&>(mac[0]));
        return key;
    }
};


typedef std::unordered_map<MAC, bool, Hash> HashMap;


MAC GetRandomMAC()
{
    return MAC({{ GetRandomByte(), GetRandomByte(), GetRandomByte(), GetRandomByte(), GetRandomByte(), GetRandomByte() }});
}


template<typename ContainerType>
std::tuple<unsigned, unsigned, unsigned> Benchmark(const ContainerType & container)
{

    // Generate a list of random MAC addresses
    std::size_t cNumIterations = 1000000;


    static auto randomMacs = [=]() -> std::vector<MAC> {
        std::vector<MAC> result;
        for (unsigned i = 0; i < cNumIterations; ++i)
        {
            result.push_back(GetRandomMAC());
        }
        return result;
    }();
    // Search each random mac in the container and count the results.
    // (The counter was introduced to prevent the compiler from optimizing everything away.)
    double beginTime = GetCurrentTime();
    unsigned found = 0;
    for (std::size_t idx = 0; idx < cNumIterations; ++idx)
    {
        if (container.find(randomMacs[idx]) != container.end())
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
    unsigned cMAX = 512;
    auto randomMacs = [&]() -> std::vector<MAC> {
        std::vector<MAC> macs;
        for (std::size_t idx = 0; idx != cMAX; ++idx) {
            macs.push_back(GetRandomMAC());
        }
        return macs;
    }();

    auto GetMap = [&](std::size_t inSize) -> Map {
        Map result;
        for (std::size_t idx = 0; idx != inSize; ++idx)
        {
            result.insert(std::make_pair(randomMacs[idx % randomMacs.size()], false));
        }
        return result;
    };

    auto GetHashMap = [&](std::size_t inSize) -> HashMap {
        HashMap result;
        for (std::size_t idx = 0; idx != inSize; ++idx)
        {
            result.insert(std::make_pair(randomMacs[idx % randomMacs.size()], false));
        }
        return result;
    };

    std::cout << "size, map/hash" << std::endl;

    std::cout << std::right << std::setw(10) << std::setfill(' ') << "Size" << std::flush
              << std::right << std::setw(10) << "Map" << std::setw(20) << " " << std::flush
              << std::right << std::setw(10) << "Hash" << std::setw(20) << " " << std::endl;

    for (unsigned i = 1; i <= cMAX; i *= 2)
    {
        std::cout << std::right << std::setw(10) << std::setfill(' ') << i << std::flush
                  << std::right << Benchmark(GetMap(i)) << std::flush
                  << std::right << Benchmark(GetHashMap(i)) << std::endl;
    }
}
