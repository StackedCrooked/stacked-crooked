#include <array>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <stdint.h>
#include <deque>

//! Decodes the network data to the request type.
//! Endianness conversion will be performed if T is an unsigned
//! fixed-width integer type (e.g. uint16_t, uint32_t or uint64_t).
template<typename T>
T decode(const uint8_t * data);

template<typename T>
struct identity { typedef T type; };

//! Decodes the network-encoded data to a host-encoded 16-bit integer.
uint16_t decode_impl(const uint8_t * data, const identity<uint16_t>&)
{
    return data[0] << 8 | data[1];
}

//! Decodes the network-encoded data to a host-encoded 32-bit integer.
uint32_t decode_impl(const uint8_t * data, const identity<uint32_t>&)
{
    return decode<uint16_t>(data) << 16 | decode<uint16_t>(data + 2);
}

//! Decodes the network-encoded data to a host-encoded 64-bit integer.
uint64_t decode_impl(const uint8_t * data, const identity<uint64_t>&)
{
    return uint64_t(decode<uint32_t>(data)) << 32 | decode<uint32_t>(data + 4);
}

//! Fallback decoder does not take endianness into account.
template<typename T>
T decode_impl(const uint8_t * data, const identity<T>&)
{
    T t;
    std::copy(data, data + sizeof(T), reinterpret_cast<char*>(&t));
    return t;
}

template<typename T>
T decode(const uint8_t * data)
{
    return decode_impl(data, identity<T>());
}

void print_binary(const uint8_t * data, unsigned length)
{
    for (unsigned i = 0; i != length; ++i)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << int(data[i]) << " ";
    }
    std::cout << std::endl;
}

template<typename T>
void check(const uint8_t * data, T value)
{
    union { T t; char c[sizeof(t)]; };
    t = value;
    if (std::is_integral<T>::value)
    {
        std::reverse(c, c + sizeof(T));
    }
    assert(!memcmp(data, c, sizeof(T)));
}

int main()
{
    uint8_t network_data_[] = { 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
    uint8_t * network_data = network_data_ + 1; // force non-aligned
    
    // print the buffer for reference
    std::cout << "network_data: ";
    print_binary(network_data, 8);
    
    {
        std::cout << "parse 16-bit integer: ";
        auto n = decode<uint16_t>(network_data);
        check(network_data, n);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse 32-bit integer: ";
        auto n = decode<uint32_t>(network_data);
        check(network_data, n);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse 64-bit integer: ";
        auto n = decode<uint64_t>(network_data);
        check(network_data, n);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse 64-bit non-integral object: ";
        struct obj { uint16_t s[4]; };
        auto n = decode<obj>(network_data);
        check(network_data, n);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        std::cout << "parse array of length 4: ";
        typedef std::array<uint8_t, 4> ip_address;
        auto n = decode<ip_address>(network_data);
        check(network_data, n);
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
}
