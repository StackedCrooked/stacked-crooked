#include <array>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>


template<typename T> struct identity { typedef T type; };


template<typename T> struct type_info;

#define GENERATE_TYPEINFO(Type) \
    template<> struct type_info<Type> { static const char * name() { return #Type; } };

GENERATE_TYPEINFO(uint8_t)
GENERATE_TYPEINFO(uint16_t)
GENERATE_TYPEINFO(uint32_t)
GENERATE_TYPEINFO(uint64_t)

template<unsigned N> struct uint_with_size;

template<> struct uint_with_size<1u> { typedef uint8_t type; };
template<> struct uint_with_size<2u> { typedef uint16_t type; };
template<> struct uint_with_size<4u> { typedef uint32_t type; };
template<> struct uint_with_size<8u> { typedef uint64_t type; };


template<typename T>
const uint8_t * get_binary(const T & t)
{
    return static_cast<const uint8_t *>(static_cast<const void*>(&t));
}


template<typename T>
char * get_binary_rw(T & t)
{
    return const_cast<char*>(get_binary(static_cast<const T&>(t)));
}


template<typename T>
std::string make_hex(const T & t)
{
    std::stringstream ss;
    const uint8_t * c = get_binary(t);
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


template<typename T> T make(const uint8_t * data);

uint8_t get_copy(const uint8_t * data, const identity<uint8_t> &)
{
    return data[0];    
}


template<typename T> struct half_of;
template<> struct half_of<uint64_t> { typedef uint32_t type; };
template<> struct half_of<uint32_t> { typedef uint16_t type; };
template<> struct half_of<uint16_t> { typedef uint8_t type; };

template<typename T>
T get_copy(const uint8_t * data, const identity<T> &)
{
    enum { 
        numbytes = sizeof(T),
        half_numbytes = numbytes / 2,
        numbits = 8 * numbytes,
        half_numbits = numbits / 2
    };
    
    typedef typename uint_with_size<half_numbytes>::type half_type;
    
    half_type left = make<half_type>(&data[0]);
    half_type right = make<half_type>(&data[half_numbytes]);
    
    union Helper
    {
        half_type halves[2];
        T t;
    };
    
    Helper helper;
    helper.halves[0] = left;
    helper.halves[1] = right;
    return helper.t;
}


void get_copy(const uint8_t * data, unsigned char & c)
{
    c = data[0];
}

template<typename T>
T make(const uint8_t * data)
{
    return get_copy(data, identity<T>());
}


struct AB8
{
    uint8_t a, b;
    
    friend AB8 make(const uint8_t * c, const identity<AB8>&)
    {
        return { c[0], c[1] };
    }
};


template<typename T>
struct BasicAB
{
    T a, b;
    
    friend BasicAB<T> make(const uint8_t * c, const identity< BasicAB<T> > &)
    {
        typedef typename half_of<T>::type half_type;        
        return { make<half_type>(c), make<half_type>(c + sizeof(half_type)) };
    }
};


typedef BasicAB<uint64_t> AB64;
typedef BasicAB<uint32_t> AB32;
typedef BasicAB<uint16_t> AB16;



template<typename T>
void assert_eq(const T & a, const T & b)
{
    std::cout << "assert_eq(" << make_string(a) << ", " << make_string(b) << ")" << std::flush;
    std::cout << ((0 == memcmp(&a, &b, sizeof(T))) ? " => PASS" : " => FAIL") << std::endl;
}


int main()
{
    {
        AB8 ab8 = { 0xab, 0xcd };
        assert_eq(make<AB8>(get_binary(ab8)), ab8);
    }
    
    {
        AB16 ab16 = { 0x2301, 0x6745 };
        assert_eq(make<AB16>(get_binary(ab16)), ab16);
    }
    
    {
        AB32 ab32 = { 0x67452301, 0xEFCDAB89 };
        assert_eq(make<AB32>(get_binary(ab32)), ab32);
    }
    
    {
        AB64 ab64 = { 0xEFCDAB8967452301, 0x01020304010203FF };
        assert_eq(make<AB64>(get_binary(ab64)), ab64);
    }
}
