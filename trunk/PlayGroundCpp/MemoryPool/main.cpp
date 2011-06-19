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
struct Pool
{
    static Pool<T> & Get()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        if (sInstances.empty())
        {
            throw std::logic_error("There is no pool.");
        }
        return *sInstances.back();
    }

    Pool(std::size_t inItemCount) :
        mData(inItemCount * sizeof(T)),
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


template<typename T>
std::vector<Pool<T>*> Pool<T>::sInstances;


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


    inline Pool<T> & pool()
    {
        return Pool<T>::Get();
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
        return pool().create(n);
    }

    inline void deallocate(pointer p, size_type n)
    {
        pool().destroy(p, n);
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
    std::cout << __PRETTY_FUNCTION__ << std::endl << std::flush;
    return true; // types are equal
}

template<typename T>
inline bool operator!=(const allocator<T> & lhs, const allocator<T> & rhs)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl << std::flush;
    return !operator==(lhs, rhs);
}

template<typename T, typename U>
inline bool operator==(const allocator<T> &, const allocator<U> & )
{
    std::cout << __PRETTY_FUNCTION__ << std::endl << std::flush;
    return false; // types are not equal
}

template<typename T, typename U>
inline bool operator!=(const allocator<T> & lhs, const allocator<U> & rhs)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl << std::flush;
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

void Insert(NormalVector & container, const Data & data) { container.push_back(data); }
void Insert(PoolVector & container,   const Data & data) {   container.push_back(data); }

void Insert(NormalSet & container, const Data & data) {   container.insert(data); }
void Insert(PoolSet & container,   const Data & data) {   container.insert(data); }


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


int main()
{
    static const std::size_t numOuterLoopIterations = 1000;
    static const std::size_t numInnerLoopIterations = 1000;

    Poco::Timestamp::TimeDiff normal = 0;
    Poco::Timestamp::TimeDiff pool = 0;

    // These counters are used to prevent GCC from optimizating out the entire test routine.
    std::size_t normalCounter = 0;
    std::size_t poolCounter = 0;


    nonstd::Pool< Data > theScopedPool(numOuterLoopIterations);

    for (std::size_t idx = 0; idx < numOuterLoopIterations; ++idx)
    {
        normal += TestPerformance<NormalVector>(numInnerLoopIterations, normalCounter);
        pool   += TestPerformance<PoolVector>(numInnerLoopIterations, poolCounter);

        normal += TestPerformance<NormalSet>(numInnerLoopIterations, normalCounter);
        pool   += TestPerformance<PoolSet>(numInnerLoopIterations, poolCounter);
    }

    std::cout << "Total non-pool size: " << normalCounter << std::endl;
    std::cout << "Total pool size: " << poolCounter << std::endl << std::endl;

    std::cout << "Normal: " << ConvertToMs(normal) << " ms" << std::endl;
    std::cout << "Pool: " << ConvertToMs(pool) << " ms" << std::endl;
    return 0;
}
