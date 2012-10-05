#include <array>
#include <cassert>
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
          | (uint64_t(c[6]) << 8)  | (uint64_t(c[7]) << 0);
}

template<typename T>
void host_decode(const uint8_t * data, T &value)
{
    union Helper
    {
        T v;
        uint8_t bytes[sizeof(value)];
    };
    
    Helper helper;
    std::copy(data, data + sizeof(value), helper.bytes);
    
    // NOTE:
    // According to the C++ standard reading from a different union
    // member than the last one written to results in undefined behavior.
    // However, both GCC and Visual Studio explicitly allow this.
    value = helper.v;
}

template<typename T>
T net_decode(const uint8_t * data, typename std::enable_if<std::is_integral<T>::value>::type * = 0)
{
        T t;
        net_decode(data, t);
        return t;
}

template<typename T>
T net_decode(const uint8_t * data, typename std::enable_if<!std::is_integral<T>::value>::type* = 0)
{
        union Helper
        {
            T t;
            uint8_t bytes[sizeof(T)];
        };
        
        Helper helper;
        std::copy(data, data + sizeof(T), helper.bytes);
        
        // NOTE:
        // According to the C++ standard reading from a different union
        // member than the last one written to results in undefined behavior.
        // However, both GCC and Visual Studio explicitly allow this.
        return helper.t;
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

#ifndef ALIGNMENT
#define ALIGNMENT 1
#endif
 
int main()
{
    std::cout << "Testing with alignment: " << ALIGNMENT << std::endl;
    
#if ALIGNMENT == 8
    // create storage object with 8-byte alignment
    uint64_t bignum = 0;
 
    // get a byte buffer to the storage
    uint8_t * network_data = static_cast<uint8_t*>(static_cast<void*>(&bignum));
    
    // fill the buffer
    for (int i = 0; i != sizeof(uint64_t); ++i) { network_data[i] = uint8_t(i); }
    
#elif ALIGNMENT == 2
    
    uint8_t network_data[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };

#elif ALIGNMENT == 1
    // storage object has 8-byte alignment.
    // our contents starts at offset-1 and therefore is misaligned.
    typedef uint64_t uint64_array[2];    
    uint64_array the_array;    
    uint8_t * the_array_data = reinterpret_cast<uint8_t*>(&the_array);
    {
        const uint8_t cvalues[] = { 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        std::copy(&cvalues[0], &cvalues[0] + sizeof(the_array), the_array_data);
    }    
    uint8_t * network_data = the_array_data + 1;

#endif

    enum { size = 8 };
    
    // print the buffer for reference
    std::cout << "network_data: ";
    print_binary(network_data, size);
    
    {
        std::cout << "16-bit integer: ";
        auto n = net_decode<uint16_t>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "32-bit integer: ";
        auto n = net_decode<uint32_t>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "64-bit integer: ";
        auto n = net_decode<uint64_t>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "64-bit object that is not an integer type: ";
        struct obj { uint16_t s[4]; };
        auto n = net_decode<obj>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "ip address: ";
        typedef std::array<uint8_t, 4> ip_address;
        auto n = net_decode<ip_address>(network_data);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
}
