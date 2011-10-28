#include <iostream>


template<typename T>
struct Next;


template<> struct Next<char > { typedef short      Type; };
template<> struct Next<short> { typedef int        Type; };
template<> struct Next<int  > { typedef long       Type; };
template<> struct Next<long > { typedef long long  Type; };


template<> struct Next<unsigned char > { typedef unsigned short      Type; };
template<> struct Next<unsigned short> { typedef unsigned int        Type; };
template<> struct Next<unsigned int  > { typedef unsigned long       Type; };
template<> struct Next<unsigned long > { typedef unsigned long long  Type; };


template<unsigned n, typename T, unsigned size = sizeof(T)>
struct FindTypeHelper
{
    typedef typename Next<T>::Type NextType;
    typedef typename FindTypeHelper<n, NextType>::Type Type;
};

// Specialization for n == sizeof(T)
template<unsigned n, typename T>
struct FindTypeHelper<n, T, n>
{
    typedef T Type;
};


template<unsigned n>
struct FindSignedType
{
    typedef typename FindTypeHelper<n, char>::Type Type;
};


template<unsigned n>
struct FindUnsignedType
{
    typedef typename FindTypeHelper<n, unsigned char>::Type Type;
};


template<unsigned n>
struct Unsigned
{
    typedef typename FindUnsignedType<n>::Type Type;
};


template<unsigned n>
struct Signed
{
    typedef typename FindSignedType<n>::Type Type;
};


typedef Unsigned<1>::Type UInt8;
typedef Unsigned<2>::Type UInt16;
typedef Unsigned<4>::Type UInt32;
typedef Unsigned<8>::Type UInt64;

typedef Signed<1>::Type Int8;
typedef Signed<2>::Type Int16;
typedef Signed<4>::Type Int32;
typedef Signed<8>::Type Int64;

int main()
{
    std::cout << "sizeof(int) = " << sizeof(int) << std::endl;
    std::cout << "sizeof(long) = " << sizeof(long) << std::endl;

    std::cout << "sizeof(UInt8) = " << sizeof(UInt8) << std::endl;
    std::cout << "sizeof(UInt16) = " << sizeof(UInt16) << std::endl;
    std::cout << "sizeof(UInt32) = " << sizeof(UInt32) << std::endl;
    std::cout << "sizeof(UInt64) = " << sizeof(UInt64) << std::endl;

    std::cout << "sizeof(Int8) = " << sizeof(Int8) << std::endl;
    std::cout << "sizeof(Int16) = " << sizeof(Int16) << std::endl;
    std::cout << "sizeof(Int32) = " << sizeof(Int32) << std::endl;
    std::cout << "sizeof(Int64) = " << sizeof(Int64) << std::endl;
    return 0;
}

