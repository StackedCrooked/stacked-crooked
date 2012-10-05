#include <array>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <stdint.h>


template<typename T>
char * binary_cast(T & t)
{
    return static_cast<char*>(static_cast<void*>(&t));
}


template<typename T>
const char * binary_cast(const T & t)
{
    return static_cast<const char*>(static_cast<const void*>(&t));
}


void decode(const uint8_t * data, uint8_t & value)
{
    value = data[0];
}


void decode(const uint8_t * data, uint16_t & value)
{       
    value = data[0] << 8 | data[1];
}


template<typename T>
T decode(const uint8_t * data);
 

void decode(const uint8_t * data, uint32_t & value)
{
    value = decode<uint16_t>(data) << 16 | decode<uint16_t>(data + 2);
}


void decode(const uint8_t * data, uint64_t & value)
{
    value = uint64_t(decode<uint32_t>(data)) << 32 | decode<uint32_t>(data + 4);
}


//! Fallback method for non-integral types.
template<typename T>
void decode(const uint8_t* data, T & t)
{
    static_assert(!std::is_integral<T>::value, "");
    std::copy(data, data + sizeof(t), binary_cast(t));
}


template<typename T>
T decode(const uint8_t * data)
{
        T t;
        decode(data, t);
        return t;
}


void print_binary(std::ostream & os, const uint8_t * data, unsigned length)
{
    for (unsigned i = 0; i != length; ++i)
    {
        os << std::hex << std::setw(2) << std::setfill('0') << int(data[i]) << " ";
    }
    os << std::endl;
}

 
void print_binary(const uint8_t * data, unsigned length)
{
    print_binary(std::cout, data, length);
}

 
int main()
{
    uint8_t network_data[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
    
    // print the buffer for reference
    std::cout << "network_data: ";
    print_binary(network_data, 8);
    
    {
        std::cout << "parse 16-bit integer: ";
        auto n = decode<uint16_t>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse 32-bit integer: ";
        auto n = decode<uint32_t>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse 64-bit integer: ";
        auto n = decode<uint64_t>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse 64-bit non-integral object: ";
        struct obj { uint16_t s[4]; };
        auto n = decode<obj>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse array of length 4: ";
        typedef std::array<uint8_t, 4> ip_address;
        auto n = decode<ip_address>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
}
