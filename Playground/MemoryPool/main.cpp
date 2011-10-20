#include "Poco/Stopwatch.h"
#include "allocator.h"
#include "Pool.h"
#include <boost/static_assert.hpp>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


struct Character
{
    Character(char c) : value(c) {}

    char value;
};


struct AB
{
    AB() : a('a'), b('b') {}

    char a, b;
};


struct XYZ
{
    XYZ() : x('x'), y('y'), z('z') {}

    char x, y, z;
};


void TestAlignment()
{
    std::cout << "sizeof(Char): " << sizeof(Character) << std::endl;
    std::cout << "sizeof(AB): " << sizeof(AB) << std::endl;

    Pool pool(6);

    // Allocate Character
    Character * character = new (pool.allocate<Character>()) Character('c');
    std::cout << "Usage: " << pool.used() << std::endl;
    assert(character->value == 'c');

    // Allocate AB
    AB * ab = new (pool.allocate<AB>()) AB();
    std::cout << "Usage: " << pool.used() << std::endl;
    assert(ab->a == 'a');
    assert(ab->b == 'b');
    assert(character->value == 'c');

    // Allocate XYZ
    XYZ * xyz = new (pool.allocate<XYZ>()) XYZ();
    std::cout << "Usage: " << pool.used() << std::endl;
    assert(xyz->x == 'x');
    assert(xyz->y == 'y');
    assert(xyz->z == 'z');
    assert(ab->a == 'a');
    assert(ab->b == 'b');
    assert(character->value == 'c');
}


void SelfTest()
{
    Pool pool(1024 * 1024); // 1 MB

    nonstd::string test = "Hello";
    assert(test == "Hello");
    assert(test.size() == std::string("Hello").size());

    nonstd::string underline(test.size(), '-');
    assert(underline == "-----");

    std::vector<char, std::allocator<char> > v1(26);
    std::vector<char, nonstd::allocator<char> > v2(26);
    assert(v1.size() == v2.size());
    assert(v2.capacity() == v2.capacity());
    for (std::size_t idx = 0; idx < 26; ++idx)
    {
        char c = 'a' + idx;
        v1.at(idx) = v2.at(idx) = c;
        assert(v1.at(idx) == v2.at(idx));
    }
    assert(!memcmp(v1.data(), v2.data(), v1.size()));
}


template<unsigned n>
struct Buffer
{
    char data[n];
};


template<unsigned n>
bool operator< (const Buffer<n> & lhs, const Buffer<n> & rhs)
{
    return lhs.data < rhs.data;
}


typedef Buffer<100> Data;


typedef std::vector<Data, std::allocator<Data>    > NormalVector;
typedef std::vector<Data, nonstd::allocator<Data> > PoolVector;

typedef std::set<Data, std::less<Data>, std::allocator<Data>    > NormalSet;
typedef std::set<Data, std::less<Data>, nonstd::allocator<Data> > PoolSet;

typedef std::map<unsigned, Data, std::less<unsigned>,    std::allocator<std::pair<const unsigned, Data> > > NormalMap;
typedef std::map<unsigned, Data, std::less<unsigned>, nonstd::allocator<std::pair<const unsigned, Data> > > PoolMap;

inline void Insert(NormalVector & container, const Data & data) { container.push_back(data); }
inline void Insert(PoolVector & container,   const Data & data) {   container.push_back(data); }

inline void Insert(NormalSet & container, const Data & data) {   container.insert(data); }
inline void Insert(PoolSet & container,   const Data & data) {   container.insert(data); }

inline void Insert(NormalMap & container, const std::pair<const unsigned, Data> & data) { container.insert(data); }
inline void Insert(PoolMap & container,   const std::pair<const unsigned, Data> & data) { container.insert(data); }

template<class ContainerType>
Poco::Timestamp::TimeDiff TestPerformance(std::size_t n, std::size_t & size)
{
    typedef typename ContainerType::value_type Data;

    Poco::Timestamp::TimeDiff result = 0;

    ContainerType container;
    for (std::size_t idx = 0; idx < n; ++idx)
    {
        Poco::Stopwatch s;
        s.start();
        Insert(container, Data());
        result += s.elapsed();

    }
    size += container.size();

    return result;
}


unsigned ConvertToMs(Poco::Timestamp::TimeDiff inDuration)
{
    return static_cast<unsigned>(inDuration / 1000);
}


void PrintResults(const std::string & inTitle,
                  Poco::Timestamp::TimeDiff inNormalTime,
                  Poco::Timestamp::TimeDiff inPoolTime)
{
    std::cout << inTitle << std::endl;
    std::cout << std::string(inTitle.size(), '=') << std::endl;
    std::cout << "Normal  : " << ConvertToMs(inNormalTime) << " ms" << std::endl;
    std::cout << "Pool    : " << ConvertToMs(inPoolTime) << " ms" << std::endl;
    std::cout << "Speedup : " << std::setprecision(2) << double(inNormalTime) / double(inPoolTime) << std::endl;
    std::cout << std::endl;
}


void Benchmark()
{
    static const std::size_t numOuterLoopIterations = 10;
    static const std::size_t numInnerLoopIterations = 10000;

    Poco::Stopwatch timer;
    timer.start();

    Poco::Timestamp::TimeDiff normal_vector = 0;
    Poco::Timestamp::TimeDiff pool_vector = 0;
    Poco::Timestamp::TimeDiff normal_map = 0;
    Poco::Timestamp::TimeDiff pool_map = 0;
    Poco::Timestamp::TimeDiff normal_set = 0;
    Poco::Timestamp::TimeDiff pool_set = 0;

    // These counters are used to prevent GCC from optimizating out the entire test routine.
    std::size_t normalCounter = 0;
    std::size_t poolCounter = 0;

    Pool theScopedPool(10 * 1024 * 1024); // 10 MB

    std::cout << "Pool creation took " << ConvertToMs(timer.elapsed()) << " ms. " << std::endl;
    timer.restart();

    for (std::size_t idx = 0; idx < numOuterLoopIterations; ++idx)
    {
        normal_vector += TestPerformance<NormalVector>(numInnerLoopIterations, normalCounter);
        pool_vector   += TestPerformance<PoolVector>(numInnerLoopIterations, poolCounter);

        normal_map += TestPerformance<NormalMap>(numInnerLoopIterations, normalCounter);
        pool_map   += TestPerformance<PoolMap>(numInnerLoopIterations, poolCounter);

        normal_set += TestPerformance<NormalSet>(numInnerLoopIterations, normalCounter);
        pool_set   += TestPerformance<PoolSet>(numInnerLoopIterations, poolCounter);
    }

    std::clog << "Counters: " << normalCounter << ", " << poolCounter << std::endl;
    std::cout << "Total time: " << ConvertToMs(timer.elapsed()) << " ms. " << std::endl << std::endl;
    PrintResults("Vector Test", normal_vector, pool_vector);
    PrintResults("Map Test", normal_map, pool_map);
    PrintResults("Set Test", normal_set, pool_set);
}


int main()
{
    TestAlignment();
    SelfTest();
    Benchmark();
    return 0;
}
