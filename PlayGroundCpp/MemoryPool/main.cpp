#include <cassert>
#include <ctime>
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
        if (sInstances.empty())
        {
            throw std::logic_error("There is no pool.");
        }
        return *sInstances.back();
    }

    Pool(std::size_t inItemCount) :
        mData(inItemCount * sizeof(T)),
        mUsed(0)
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
    inline void destroy(T * , std::size_t )
    {
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
    static std::vector< Pool<T>* > sInstances;
};


template<typename T>
std::vector< Pool<T>* > Pool<T>::sInstances;


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

    inline Pool<T> & pool()
    {
        return Pool<T>::Get();
    }

    template<typename U>
    struct rebind
    {
        typedef allocator<U> other;
    };

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

    inline bool operator==(const allocator<T> & ) const
    {
        return true;
    }

    inline bool operator!=(const allocator<T> & ) const
    {
        return false;
    }
};


typedef std::basic_string< char, std::char_traits<char>, nonstd::allocator<char> > string;


} // namespace nonstd


clock_t TestPerformance(std::size_t inNumIterations)
{
    clock_t start = clock();

    long long totalSize = 0;

    std::vector<std::size_t> v;
    for (std::size_t idx = 0; idx < inNumIterations; ++idx)
    {
        v.push_back(idx);
        totalSize += v.size();
    }

    return start - clock();
}


clock_t TestPerformanceWithPool(std::size_t inNumIterations)
{
    clock_t start = clock();

    nonstd::Pool<std::size_t> pool(100 * 1000);
    long long totalSize = 0;
    std::vector<std::size_t, nonstd::allocator<std::size_t> > v;
    for (std::size_t idx = 0; idx < inNumIterations; ++idx)
    {
        v.push_back(idx);
        totalSize += v.size();
    }

    return start - clock();
}


unsigned ConvertToMs(clock_t inDuration)
{
    double result = static_cast<double>(inDuration) / static_cast<double>(CLOCKS_PER_SEC);
    result /= 1000.0;
    return static_cast<unsigned>(0.5 + result);
}


int main()
{

    std::size_t numIterations = 1000;

    clock_t normal = 0;
    clock_t pool = 0;

    for (std::size_t idx = 0; idx < 1000; ++idx)
    {
        normal += TestPerformance(numIterations);
        pool += TestPerformanceWithPool(numIterations);
    }

    std::cout << "Normal: " << ConvertToMs(normal) << std::endl;
    std::cout << "Pool: " << ConvertToMs(pool) << std::endl;
    return 0;
}
