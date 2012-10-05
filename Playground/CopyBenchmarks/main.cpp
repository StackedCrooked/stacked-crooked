#include <array>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <stdint.h>

template<typename T, typename std::enable_if<std::is_integral<T>::value>::type* = 0>
T net_decode(const uint8_t * data);

template<typename T, typename std::enable_if<!std::is_integral<T>::value>::type* = 0>
T net_decode(const uint8_t * data);

void net_decode(const uint8_t * data, uint16_t & value)
{       
        value = data[0] << 8 | data[1];
}
 
void net_decode(const uint8_t * data, uint32_t & value)
{           
    value = data[0] << 24 | data[1] << 16 | data[2] <<  8 | data[3] <<  0;
}

void net_decode(const uint8_t * c, uint64_t & value)
{       
    value = (uint64_t(c[0]) << 56) | (uint64_t(c[1]) << 48)
          | (uint64_t(c[2]) << 40) | (uint64_t(c[3]) << 32)
          | (uint64_t(c[4]) << 24) | (uint64_t(c[5]) << 16)
          | (uint64_t(c[6]) <<  8)  | (uint64_t(c[7]) << 0);
}

template<typename T>
T net_decode(const uint8_t * data, typename std::enable_if<std::is_integral<T>::value>::type * = 0)
{
        T t;
        net_decode(data, t);
        return t;
}

template<typename T>
T net_decode(const uint8_t * source, typename std::enable_if<!std::is_integral<T>::value>::type* = 0)
{
    T t;
    std::copy(source, source + sizeof(T), reinterpret_cast<uint8_t*>(&t));
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
        auto n = net_decode<uint16_t>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse 32-bit integer: ";
        auto n = net_decode<uint32_t>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse 64-bit integer: ";
        auto n = net_decode<uint64_t>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse 64-bit non-integral object: ";
        struct obj { uint16_t s[4]; };
        auto n = net_decode<obj>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse array of length 4: ";
        typedef std::array<uint8_t, 4> ip_address;
        auto n = net_decode<ip_address>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
}
