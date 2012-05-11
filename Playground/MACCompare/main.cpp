#include <boost/lexical_cast.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <typeinfo>
#include <stdexcept>
#include <tuple>
#include <cxxabi.h>
#include <sys/time.h>


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


double GetCurrentTime()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return double (tv.tv_sec) + 0.000001 * tv.tv_usec;
}


std::uint8_t* Random();


struct Memcmp
{
    Memcmp(const std::uint8_t * inData)
    {
        memcpy(key, inData, sizeof(key));
    }

    friend bool operator<(const Memcmp & lhs, const Memcmp & rhs)
    { return std::memcmp(lhs.key, rhs.key, 6) < 0; }

    std::uint8_t key[6];
};


struct Pair
{
    Pair(const std::uint8_t * inData) :
        key(*reinterpret_cast<const std::uint32_t*>(inData),
            *reinterpret_cast<const std::uint16_t*>(inData + 4))
    {
    }

    friend bool operator<(const Pair & lhs, const Pair & rhs)
    { return lhs.key < rhs.key; }

    std::pair<std::uint32_t, std::uint16_t> key;
};


struct ReversePair
{
    ReversePair(const std::uint8_t * inData) :
        key(*reinterpret_cast<const std::uint16_t*>(inData),
            *reinterpret_cast<const std::uint32_t*>(inData + 2))
    {
    }

    friend bool operator<(const ReversePair & lhs, const ReversePair & rhs)
    { return lhs.key < rhs.key; }

    std::pair<std::uint16_t, std::uint32_t> key;
};


struct Single64
{
    Single64(const std::uint8_t * inData) :
        key(0xFFFFFFFFFFFF0000 & *reinterpret_cast<const std::uint64_t*>(inData))
    {
    }

    friend bool operator<(const Single64 & lhs, const Single64 & rhs)
    { return lhs.key < rhs.key; }

    std::uint64_t key;
};



struct Cheater
{
    Cheater(const std::uint8_t *) :
        mKey(count())
    {
    }

    friend bool operator<(const Cheater & lhs, const Cheater & rhs)
    {
        return std::string("abcabcabc" + boost::lexical_cast<std::string>(lhs.mKey))
                <
               std::string("abcabcabc" + boost::lexical_cast<std::string>(rhs.mKey));
    }

    static unsigned count()
    {
        static unsigned fCount = 0;
        return fCount++;
    }

    unsigned mKey;
};


template<typename T>
std::map<T, bool> GetMapping()
{
    std::map<T, bool> mapping;
    for (std::size_t idx = 0; idx < 100; ++idx)
    {
        mapping.insert(std::make_pair(T(Random()), true));
    }
    std::cout << "Size of mapping: " << mapping.size() << std::endl;
    return mapping;
}

template<typename T>
std::tuple<double, unsigned, unsigned> Benchmark()
{
    std::cout << demangle(typeid(T).name()) << ": ";


    static std::map<T, bool> fMapping = GetMapping<T>();

    std::tuple<double, unsigned, unsigned> result;
    double & time = std::get<0>(result);
    unsigned & found = std::get<1>(result);
    unsigned & unfound = std::get<2>(result);



    double beginTime = GetCurrentTime();
    for (std::size_t idx = 0; idx != 1000; ++idx)
    {
        if (fMapping.find(T(Random())) != fMapping.end())
        {
            found++;
        }
        else
        {
            unfound++;
        }
    }
    time = unsigned(0.5 + 1000.0 * (GetCurrentTime() - beginTime));
    return result;
}


std::ostream & operator<<(std::ostream & os, const std::tuple<double, unsigned, unsigned> & result)
{
    return os << "Time: " << std::get<0>(result) << " (" << std::get<1>(result) << "/" << std::get<2>(result) << ")";
}


int main()
{
    std::cout << Benchmark<Memcmp>() << std::endl;
    std::cout << Benchmark<Pair>() << std::endl;
    std::cout << Benchmark<ReversePair>() << std::endl;
    std::cout << Benchmark<Single64>() << std::endl;
    std::cout << Benchmark<Cheater>() << std::endl;
}
