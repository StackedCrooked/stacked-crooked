#include <cassert>
#include <string>
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


template<class T>
struct Pool
{
    static Pool<T> & SharedInstance()
    {
        static Pool<T> fInstance(1024);
        return fInstance;
    }

    Pool(std::size_t inItemCount) :
        mData(inItemCount * sizeof(T)),
        mUsed(),
        mFreed(),
        mTop()
    {
        std::cout << "Create pool" << std::endl;
    }

    ~Pool()
    {
        std::cout << "Pool Stats\n----------\n";
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

    std::vector<T> mData;
    std::size_t mUsed;
    std::size_t mFreed;
    std::size_t mTop;
};


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


    template<typename U>
    struct rebind
    {
        typedef allocator<U> other;
    };

    inline pointer allocate(size_type n, typename std::allocator<void>::const_pointer = 0)
    {
        return Pool<T>::SharedInstance().create(n);
    }

    inline void deallocate(pointer p, size_type n)
    {
        Pool<T>::SharedInstance().destroy(p, n);
    }

    inline size_type max_size() const
    {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    inline void construct(pointer p, const T & t)
    {
        std::cout << "construct " << (void *)p << std::endl << std::flush;
        new(p) T(t);
    }

    inline void destroy(pointer p)
    {
        std::cout << "construct " << (void *)p << std::endl << std::flush;
        p->~T();
    }

    inline bool operator==(const allocator & rhs) const
    {
        return true; //return this == &rhs;
    }

    inline bool operator!=(const allocator & rhs) const
    {
        return this != &rhs;
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
    nonstd::string s = "s";
    nonstd::string t = "Hello World!";
    s = "s2";
    t = "t2";

    new nonstd::string("leak");

    std::cout << s << std::endl;
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
