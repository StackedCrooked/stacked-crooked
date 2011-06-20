#include "Poco/Stopwatch.h"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


namespace nonstd {


struct Pool
{
    static Pool & Get()
    {
        if (sInstances.empty())
        {
            throw std::logic_error("There is no pool.");
        }
        return *sInstances.back();
    }

    Pool(std::size_t inTotalSize) :
        mData(inTotalSize),
        mUsed(0),
        mFreed(0)
    {
        sInstances.push_back(this);
    }

    ~Pool()
    {
        sInstances.pop_back();
    }

    template<class T>
    inline T * allocate(std::size_t n = 1)
    {
        static const int segment_size = sizeof(void*);
        std::size_t size_unpadded = n * sizeof(T);
        std::size_t padding = (segment_size - (size_unpadded % segment_size)) % segment_size;
        std::size_t size = size_unpadded + padding;
        if (mUsed + size > capacity())
        {
            throw std::bad_alloc();
        }

        T * result = reinterpret_cast<T*>(&mData[mUsed]);
        mUsed += size;
        return result;
    }

    template<class T>
    inline void deallocate(T *, size_t n = 1)
    {
        mFreed += sizeof(T) * n;
        if (mFreed == mUsed)
        {
            mFreed = mUsed = 0;
        }
    }

    std::size_t used() const
    {
        return mUsed;
    }

    std::size_t capacity()
    {
        return mData.size();
    }

private:
    std::vector<char> mData;
    std::size_t mUsed;
    std::size_t mFreed;
    static std::vector< Pool* > sInstances;
};


std::vector<Pool*> Pool::sInstances;


template<class T>
inline void * Allocate(std::size_t n = 1) { return Pool::Get().allocate<T>(n); }

template<class T>
inline void Deallocate(T * t, std::size_t n = 1) { Pool::Get().deallocate<T>(t, n); }

template<typename T>
inline T * New() { return new (Allocate<T>()) T; }

template<typename T, typename A0>
inline T * New(const A0 & a0) { return new (Allocate<T>()) T(a0); }

template<typename T, typename A0, typename A1>
inline T * New(const A0 & a0, const A1 & a1) { return new (Allocate<T>()) T(a0, a1); }

template<typename T, typename A0, typename A1, typename A2>
inline T * New(const A0 & a0, const A1 & a1, const A2 & a2) { return new (Allocate<T>()) T(a0, a1, a2); }

template<typename T, typename A0, typename A1, typename A2, typename A3>
inline T * New(const A0 & a0, const A1 & a1, const A2 & a2, const A3 & a3) { return new (Allocate<T>()) T(a0, a1, a2, a3); }

template<typename T>
inline void Delete(T* t) { Deallocate<T>(t); }

template<typename T>
struct allocator
{
    typedef std::allocator<T> std_allocator;
    typedef typename std_allocator::value_type      value_type;
    typedef typename std_allocator::size_type       size_type;
    typedef typename std_allocator::difference_type difference_type;
    typedef typename std_allocator::reference       reference;
    typedef typename std_allocator::const_reference const_reference;
    typedef typename std_allocator::pointer         pointer;
    typedef typename std_allocator::const_pointer   const_pointer;

    allocator() throw() {}

    allocator(const allocator &) throw() {}

    template<class U>
    allocator(const allocator<U> & ) throw() {}

    ~allocator() throw() {}

    template<typename U>
    struct rebind
    {
        typedef allocator<U> other;
    };

    pointer       address(reference value)       const { return &value; }
    const_pointer address(const_reference value) const { return &value; }

    inline pointer allocate(size_type n, typename std::allocator<void>::const_pointer = 0)
    {
        return Pool::Get().allocate<T>(n);
    }

    inline void deallocate(pointer p, size_type n)
    {
        Pool::Get().deallocate(p, n);
    }

    inline size_type max_size() const
    {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    inline void construct(pointer p, const T & t)
    {
        new(p) T (t);
    }

    inline void destroy(pointer p)
    {
        p->~T();
    }
};


template<typename T>
inline bool operator==(const allocator<T> &, const allocator<T> & )
{
    return true; // types are equal
}

template<typename T>
inline bool operator!=(const allocator<T> & lhs, const allocator<T> & rhs)
{
    return !operator==(lhs, rhs);
}

template<typename T, typename U>
inline bool operator==(const allocator<T> &, const allocator<U> & )
{
    return false; // types are not equal
}

template<typename T, typename U>
inline bool operator!=(const allocator<T> & lhs, const allocator<U> & rhs)
{
    return !operator ==(lhs, rhs);
}


typedef std::basic_string< char, std::char_traits<char>, nonstd::allocator<char> > string;


} // namespace nonstd


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


typedef Buffer<1024> Data;


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

    Poco::Stopwatch s;
    s.start();

    ContainerType container;
    for (std::size_t idx = 0; idx < n; ++idx)
    {
        Insert(container, Data());
    }
    size += container.size();

    return s.elapsed();
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


void SelfTest()
{
    nonstd::Pool theScopedPool(1024 * 1024); // 1 MB
    using nonstd::New;

    nonstd::string test = "Hello";
    assert(test == "Hello");
    assert(test.size() == std::string("Hello").size());

    nonstd::string underline(test.size(), '-');
    assert(underline == "-----");

    std::vector<char, std::allocator<char> > v1(17);
    std::vector<char, nonstd::allocator<char> > v2(17);
    assert(v1.size() == v2.size());
    assert(v2.capacity() == v2.capacity());
    for (char c = 'a'; c != 'z'; ++c)
    {
        v1[c] = v2[c] = c;
        assert(v1[c] == v2[c]);
    }
    assert(!memcmp(v1.data(), v2.data(), v1.size()));
}


void Benchmark()
{
    static const std::size_t numOuterLoopIterations = 2000;
    static const std::size_t numInnerLoopIterations = 2000;

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

    nonstd::Pool theScopedPool(10 * 1024 * 1024); // 10 MB

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

    // Print it out, and delete it with "\r".
    std::clog << "Counters: " << normalCounter << ", " << poolCounter << std::endl;

    std::cout << "Total time: " << ConvertToMs(timer.elapsed()) << " ms. " << std::endl << std::endl;

    PrintResults("Vector Test", normal_vector, pool_vector);
    PrintResults("Map Test", normal_map, pool_map);
    PrintResults("Set Test", normal_set, pool_set);
}


int main()
{
    SelfTest();
    Benchmark();
    return 0;
}
