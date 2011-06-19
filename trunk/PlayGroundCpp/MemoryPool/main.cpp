#include "Poco/Stopwatch.h"
#include <cassert>
#include <ctime>
#include <numeric>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>


namespace nonstd {


template<class T>
struct Pool;

template<>
struct Pool<char>
{
    typedef char T;
    static Pool<T> & Get()
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

    inline T * create(std::size_t n)
    {

        if (mUsed + n > capacity())
        {
            throw std::bad_alloc();
        }

        T * result = &mData[mUsed];
        mUsed += n;
        return result;
    }

    /**
     * Does not free.
     */
    inline void destroy(T * , std::size_t n)
    {
        mFreed += n;
        if (mFreed == mUsed)
        {
            mFreed = mUsed = 0;
        }
    }

    std::size_t size() const
    {
        return mUsed;
    }

    std::size_t capacity()
    {
        return mData.size();
    }

private:
    std::vector<T> mData;
    std::size_t mUsed;
    std::size_t mFreed;
    static std::vector< Pool<T>* > sInstances;
};


std::vector<Pool<char>*> Pool<char>::sInstances;


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

    allocator(const allocator<T> &) throw() {}

    template<class U>
    allocator(const allocator<U> & ) throw() {}

    ~allocator() throw() {}


    inline Pool<char> & pool()
    {
        return Pool<char>::Get();
    }

    template<typename U>
    struct rebind
    {
        typedef allocator<U> other;
    };

    pointer       address(reference value)       const { return &value; }
    const_pointer address(const_reference value) const { return &value; }

    inline pointer allocate(size_type n, typename std::allocator<void>::const_pointer = 0)
    {
        return reinterpret_cast<pointer>(pool().create(sizeof(T) * n));
    }

    inline void deallocate(pointer p, size_type n)
    {
        pool().destroy(reinterpret_cast<char*>(p), sizeof(T) * n);
    }

    inline size_type max_size() const
    {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    inline void construct(pointer p, const T & t)
    {
        new(p) T(t);
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


typedef Buffer<128> Data;


typedef std::vector<Data, std::allocator<Data>    > NormalVector;
typedef std::vector<Data, nonstd::allocator<Data> > PoolVector;

typedef std::set<Data, std::less<Data>, std::allocator<Data>    > NormalSet;
typedef std::set<Data, std::less<Data>, nonstd::allocator<Data> > PoolSet;

inline void Insert(NormalVector & container, const Data & data) { container.push_back(data); }
inline void Insert(PoolVector & container,   const Data & data) {   container.push_back(data); }

inline void Insert(NormalSet & container, const Data & data) {   container.insert(data); }
inline void Insert(PoolSet & container,   const Data & data) {   container.insert(data); }


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
    for (std::size_t idx = 0; idx < inTitle.size(); ++idx)
    {
        std::cout << "-";
    }
    std::cout << std::endl;
    std::cout << "Normal: " << ConvertToMs(inNormalTime) << " ms" << std::endl;
    std::cout << "Pool  : " << ConvertToMs(inPoolTime) << " ms" << std::endl << std::endl;
}


int main()
{
    static const std::size_t numOuterLoopIterations = 1000;
    static const std::size_t numInnerLoopIterations = 1000;

    Poco::Stopwatch timer;
    timer.start();

    Poco::Timestamp::TimeDiff normal_vector = 0;
    Poco::Timestamp::TimeDiff pool_vector = 0;

    Poco::Timestamp::TimeDiff normal_set = 0;
    Poco::Timestamp::TimeDiff pool_set = 0;

    // These counters are used to prevent GCC from optimizating out the entire test routine.
    std::size_t normalCounter = 0;
    std::size_t poolCounter = 0;

    nonstd::Pool<char> theScopedPool(1024 * 1024);

    std::cout << "Pool creation took " << ConvertToMs(timer.elapsed()) << " ms. " << std::endl;
    timer.restart();

    for (std::size_t idx = 0; idx < numOuterLoopIterations; ++idx)
    {
        normal_vector += TestPerformance<NormalVector>(numInnerLoopIterations, normalCounter);
        pool_vector   += TestPerformance<PoolVector>(numInnerLoopIterations, poolCounter);

        normal_set += TestPerformance<NormalSet>(numInnerLoopIterations, normalCounter);
        pool_set   += TestPerformance<PoolSet>(numInnerLoopIterations, poolCounter);
    }

    std::cout << "Total time: " << ConvertToMs(timer.elapsed()) << " ms. " << std::endl;

    std::cout << "Total non-pool size: " << normalCounter << std::endl << std::endl;
    std::cout << "Total pool size: " << poolCounter << std::endl << std::endl;

    PrintResults("Vector Test", normal_vector, normal_set);

    PrintResults("Set Test", pool_vector, pool_set);
    return 0;
}
