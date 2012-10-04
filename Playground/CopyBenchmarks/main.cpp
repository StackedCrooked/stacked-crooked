#include <algorithm>
#include <array>
#include <cassert>
#include <cxxabi.h>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <typeinfo>

template<typename T> uint8_t* get_binary(T & t);
template<typename T> const uint8_t* get_const_binary(const T & t);

template<typename T>
std::string make_hex(const T & t)
{
    std::stringstream ss;
    const uint8_t* c = get_const_binary(t);
    for (unsigned i = 0; i < sizeof(t); ++i)
    {
        if (i != 0)
        {
            ss << " ";
        }
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
const uint8_t* get_const_binary(const T & t)
{
    return static_cast<const uint8_t*>(static_cast<const void*>(&t));    
}


template<typename T>
uint8_t* get_binary(T & t)
{
    return const_cast<uint8_t*>(get_const_binary(t));
}


template<typename Destination, unsigned Index, unsigned Size>
struct copy_impl;

template<typename Destination, unsigned Index, unsigned Size>
struct copy_impl
{
    void operator()(uint8_t* dst, const uint8_t* src)
    {
        dst[Index] = src[Index];
        copy_impl<Destination, Index + 1, Size>()(dst, src);
    }
};

template<typename Destination, unsigned I>
struct copy_impl<Destination, I, I>
{
    void operator()(uint8_t*, const uint8_t*) { }
};

template<typename Destination>
void copy(Destination & dst, const uint8_t* data)
{
    uint8_t* bytes = get_binary(dst);
    copy_impl<Destination, 0, sizeof(Destination)>()(bytes, data);
    
}

template<typename T>
T make(const uint8_t* data)
{
    T result;
    copy(result, data);
    return result;
}


template<typename T>
void assert_eq(const T & a, const T & b)
{
    std::cout << "assert_eq:\n\t" << make_string(a) << "\n\t" << make_string(b) << std::endl;
    std::cout << ((0 == memcmp(&a, &b, sizeof(T))) ? " => PASS" : " => FAIL") << std::endl << std::endl;
}


int main()
{
    struct Header
    {
        uint32_t i;
        uint16_t s[2];
        uint8_t c[4];
    };
    
    
    Header h1;
    uint8_t* c = reinterpret_cast<uint8_t*>(&h1);
    for (unsigned i = 0; i < sizeof(h1); ++i)
    {
        c[i] = i;
    }
    
    Header h2 = h1;
    assert_eq(h1, h2);    
    
    Header h3 = make<Header>(c);
    assert_eq(h2, h3);
}
