#include <array>
#include <cassert>
#include <cxxabi.h>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <typeinfo>

template<typename T> char * get_binary(T & t);
template<typename T> const char * get_const_binary(const T & t);

template<typename T>
std::string make_hex(const T & t)
{
    std::stringstream ss;
    const char * c = get_const_binary(t);
    ss << "0x";
    for (unsigned i = 0; i < sizeof(t); ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c[i]);        
    }
    return ss.str();
}

std::string make_string(uint8_t n)  { return static_cast<std::stringstream&>(std::stringstream() << n).str(); }
std::string make_string(uint16_t n) { return static_cast<std::stringstream&>(std::stringstream() << n).str(); }
std::string make_string(uint32_t n) { return static_cast<std::stringstream&>(std::stringstream() << n).str(); }
std::string make_string(uint64_t n) { return static_cast<std::stringstream&>(std::stringstream() << n).str(); }

template<typename T>
std::string make_string(const T & t)
{
    return make_hex(t);
}


template<typename T>
const char * get_const_binary(const T & t)
{
    return static_cast<const char *>(static_cast<const void*>(&t));    
}


template<typename T>
char * get_binary(T & t)
{
    return const_cast<char*>(get_const_binary(t));
}


template<typename Destination, unsigned Index, unsigned Size>
struct copy_impl;

template<typename Destination, unsigned Index, unsigned Size>
struct copy_impl
{
    void operator()(char * dst, const char * src)
    {
        dst[Index] = src[Index];
        copy_impl<Destination, Index + 1, Size>()(dst, src);
    }
};

template<typename Destination, unsigned Index>
struct copy_impl<Destination, Index, Index>
{
    void operator()(char *, const char * )
    {
    }
};

template<typename Destination>
void copy(Destination & dst, const char * data)
{
    char * bytes = get_binary(dst);
    copy_impl<Destination, 0, sizeof(Destination)>()(bytes, data);
    
}


template<typename T>
void assert_eq(const T & a, const T & b)
{
    std::cout << "assert_eq(" << make_string(a) << ", " << make_string(b) << ")" << std::flush;
    std::cout << ((0 == memcmp(&a, &b, sizeof(T))) ? " => PASS" : " => FAIL") << std::endl;
}


int main()
{
    struct Header
    {
        uint32_t a;
        uint16_t b, c;
    };
    
    
    Header h1 = { 1, 20, 21 };
    char * buf = get_binary(h1);    
    Header h2;
    copy(h2, buf);
    
    
    assert_eq(h1, h2);
}
