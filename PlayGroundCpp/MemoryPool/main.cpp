#include <cassert>
#include <string>
#include <set>
#include <vector>
#include <iostream>
#include <boost/lexical_cast.hpp>


namespace nonstd {


template<class T>
struct Pool
{
    static Pool<T> & Get()
    {
        if (!sInstances.empty())
        {
            return *sInstances.back();
        }
        else
        {
            static Pool<T> fSharedInstance("Shared Pool", 1024);
            return fSharedInstance;
        }
    }

    Pool(const std::string & inName, std::size_t inItemCount) :
        mName(inName),
        mData(inItemCount * sizeof(T)),
        mUsed(),
        mFreed(),
        mTop()
    {
        sInstances.push_back(this);
    }

    ~Pool()
    {
        assert(sInstances.back() == this);
        sInstances.pop_back();

        std::cout << mName << std::endl;
        for (std::size_t idx = 0; idx < mName.size(); ++idx)
        {
            std::cout << "-";
        }
        std::cout << std::endl;
        std::cout << "Type: " << typeid(T).name() << std::endl;
        std::cout << "Top load: " << sizeof(T) * mTop << " bytes" << std::endl;
        std::cout << "Leaked: " << sizeof(T) * (mUsed - mFreed) << " bytes " << std::endl << std::endl << std::flush;
    }

    T * create(std::size_t n)
    {
        T * result = &mData[mUsed];
        mUsed += n;
        assert(mUsed < capacity());
        mTop = std::max<std::size_t>(mUsed, mTop);
        return result;
    }

    /**
     * Increments the counter of freed memory.
     * Once this counter equal the amout of used memory, then both counters are set to zero.
     */
    void destroy(T * inValue, std::size_t n)
    {
        mFreed += n;
        assert(mFreed <= mUsed);
        if (mFreed == mUsed)
        {
            mFreed = 0;
            mUsed = 0;
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
    std::string mName;
    std::vector<T> mData;
    std::size_t mUsed;
    std::size_t mFreed;
    std::size_t mTop;
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


void TestString()
{
    nonstd::Pool<char> pool("Outer pool", 1024);
    nonstd::string s = "Abc";
    {
        nonstd::Pool<char> pool("Inner pool", 512);
        nonstd::string t = "def";
    }
}


void TestVector()
{
    std::vector<int, nonstd::allocator<int> > v;
    v.push_back(1);
    v.size();
    v.clear();
    v.resize(10);
    v.reserve(20);

    std::vector<int, nonstd::allocator<int> > w = v;
    v = w;

    std::vector<int, nonstd::allocator<int> >();
    std::vector<int, nonstd::allocator<int> >(1);
    std::vector<int, nonstd::allocator<int> >(10, 0);

    new std::vector<double, nonstd::allocator<double> >(1); // leak
}


int main()
{
    TestString();
    TestVector();
    return 0;
}
