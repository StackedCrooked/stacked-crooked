#include <cassert>
#include <string>
#include <set>
#include <vector>
#include <iostream>
#include <boost/lexical_cast.hpp>


namespace nonstd {

enum NoArg {};

template<
class T01 = NoArg, class T02 = NoArg, class T03 = NoArg,
      class T04 = NoArg, class T05 = NoArg, class T06 = NoArg,
      class T07 = NoArg, class T08 = NoArg, class T09 = NoArg,
      class T10 = NoArg, class T11 = NoArg, class T12 = NoArg
      >
struct ArgPack;

template<
>
struct ArgPack<
        NoArg, NoArg, NoArg, NoArg, NoArg, NoArg,
        NoArg, NoArg, NoArg, NoArg, NoArg, NoArg
        >
    {};

typedef ArgPack<
NoArg, NoArg, NoArg, NoArg, NoArg, NoArg,
       NoArg, NoArg, NoArg, NoArg, NoArg, NoArg
       >                                           EmptyArgPack;

inline ArgPack<> args()
{
    return ArgPack<>();
}

template<class T01 >
struct ArgPack<T01, NoArg, NoArg, NoArg, NoArg, NoArg,
        NoArg, NoArg, NoArg, NoArg, NoArg, NoArg>
{
    T01 const & a01;
    ArgPack(T01 const & v01) : a01(v01)
    {
    }
};

template<class T01, class T02 >
struct ArgPack<T01, T02, NoArg, NoArg, NoArg, NoArg,
        NoArg, NoArg, NoArg, NoArg, NoArg, NoArg>
{
    T01 const & a01;
    T02 const & a02;
    ArgPack(T01 const & v01, T02 const & v02) : a01(v01), a02(v02)
    {
    }
};

template< class T01 >
inline ArgPack< T01 > args(T01 const & a01) { return ArgPack< T01 >(a01); }

template< class T01, class T02 >
inline ArgPack< T01 , T02 > args(T01 const & a01, T02 const & a02) { return ArgPack< T01, T02 >(a01, a02); }



template< typename Type >
class ConstructorArgForwarder
    : public Type
{
public:
    typedef Type        Base;

    // TODO: remove
    virtual ~ConstructorArgForwarder() {}

    ConstructorArgForwarder(EmptyArgPack const &)
        : Base()
    {}

    template< class T01 >
    ConstructorArgForwarder(
        ArgPack< T01 > const & args
    )
        : Base(args.a01)
    {}

    template< class T01, class T02 >
    ConstructorArgForwarder(
        ArgPack< T01, T02 > const & args
    )
        : Base(args.a01, args.a02)
    {}

    template< class T01, class T02, class T03 >
    ConstructorArgForwarder(
        ArgPack< T01, T02, T03 > const & args
    )
        : Base(args.a01, args.a02, args.a03)
    {}

    // And more, up to max 12 arguments.
};


struct PoolBase
{
    PoolBase()
    {
    }

    virtual ~PoolBase()
    {
    }

    static std::set<PoolBase*> mPools;
};


template<class T>
struct Pool : public PoolBase
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
    typedef typename std_allocator::value_type       value_type;
    typedef typename std_allocator::size_type       size_type;
    typedef typename std_allocator::difference_type difference_type;
    typedef typename std_allocator::reference       reference;
    typedef typename std_allocator::const_reference const_reference;
    typedef typename std_allocator::pointer      pointer;
    typedef typename std_allocator::const_pointer    const_pointer;

    Pool<T> & GetPool()
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
        return GetPool().create(n);
    }

    inline void deallocate(pointer p, size_type n)
    {
        GetPool().destroy(p, n);
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


template<typename T>
class vector : public ConstructorArgForwarder< std::vector<T, nonstd::allocator<T> > >
{
public:
    typedef std::vector<T, nonstd::allocator<T> > VectorBase;
    typedef ConstructorArgForwarder< VectorBase > Base;

    vector() :
        Base(EmptyArgPack())
    {
    }

    template<class Args>
    vector(const Args & args) :
        Base(args)
    {
    }

};


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

    nonstd::vector<int>();
    nonstd::vector<int>(nonstd::args(1));
    nonstd::vector<int>(nonstd::args(10, 0));

    new std::vector<double, nonstd::allocator<double> >(1); // leak
}


int main()
{
    TestString();
    TestVector();
    return 0;
}
