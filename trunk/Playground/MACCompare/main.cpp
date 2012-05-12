#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
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


std::mt19937 GetRNG()
{
    std::mt19937 rng;
    rng.seed(time(0));
    return rng;
}


uint8_t GetRandomByte()
{
    static auto rng = GetRNG();
    return std::uniform_int_distribution<uint8_t>(0, 255)(rng);
}


//! 6-byte field for emulating MAC address
typedef std::array<uint8_t, 6> MAC;


MAC GetRandomMAC()
{
    return MAC({{ GetRandomByte(), GetRandomByte(), GetRandomByte(), GetRandomByte(), GetRandomByte(), GetRandomByte() }});
}


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
typedef std::map<MAC, bool> Map;


template<typename ContainerType>
std::tuple<unsigned, unsigned, unsigned> Benchmark(const ContainerType & container)
{

    // Generate a list of random MAC addresses
    std::size_t cNumIterations = 1000000;


    static auto randomMacs = [=](){
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
    unsigned cMAX = 1000000;
    auto randomMacs = [&]() {
        std::vector<MAC> macs;
        for (std::size_t idx = 0; idx != cMAX; ++idx) {
            macs.push_back(GetRandomMAC());
        }
        return macs;
    }();

    auto GetMap = [&](std::size_t inSize) {
        Map result;
        for (std::size_t idx = 0; idx != inSize; ++idx)
        {
            result.insert(std::make_pair(randomMacs[idx % randomMacs.size()], false));
        }
        return result;
    };

    auto GetHashMap = [&](std::size_t inSize) {
        HashMap result;
        for (std::size_t idx = 0; idx != inSize; ++idx)
        {
            result.insert(std::make_pair(randomMacs[idx % randomMacs.size()], false));
        }
        return result;
    };


    std::cout << "size, map/hash" << std::endl;

    for (unsigned i = 1; i <= cMAX; i *= 2)
    {
        std::cout << std::right << std::setw(10) << std::setfill(' ') << i << std::flush
                  << std::right << Benchmark(GetMap(i)) << std::flush
                  << std::right << Benchmark(GetHashMap(i)) << std::endl;
    }
}
