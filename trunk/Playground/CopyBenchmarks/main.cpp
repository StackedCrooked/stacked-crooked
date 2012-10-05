#include <cassert>
#include <iomanip>
#include <iostream>
#include <stdint.h>

template<typename T>
T decode(const uint8_t * data);


void decode(const unsigned char * network_data, uint16_t & value)
{	
	value = network_data[0] << 8 | network_data[1]; // automagically converted to host encoded-value
}


void decode(const unsigned char * c, uint32_t & value)
{	
    value = c[0] << 24 | c[1] << 16 | c[2] <<  8 | c[3] << 0;
}


void decode(const unsigned char * c, uint64_t & value)
{	
    value = (uint64_t(c[0]) << 56)
          | (uint64_t(c[1]) << 48)
          | (uint64_t(c[2]) << 40)
          | (uint64_t(c[3]) << 32)
          | (uint64_t(c[4]) << 16)
          | (uint64_t(c[5]) << 8)
          | (uint64_t(c[6]) << 0)
          ;
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
    for (unsigned loopIndex = 0; loopIndex != length; ++loopIndex)
    {
        // print in network-encoding, so reversed order
        int dataIndex = length - loopIndex;
        
        if (dataIndex != 0)
        {
            os << " ";
        }        
        os << std::hex << std::setfill('0') << std::setw(2) << int(data[dataIndex]);
    }
    os << std::endl;
}


void print_binary(const uint8_t * data, unsigned length)
{
    print_binary(std::cout, data, length);
}


int main()
{
    // sample network encoded data (big endian)
	uint8_t network_data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    
    {
        auto n = decode<uint16_t>(network_data);
        std::cout << "16-bit: ";
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        auto n = decode<uint32_t>(network_data);
        std::cout << "32-bit: ";
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
    
    {
        auto n = decode<uint64_t>(network_data);
        std::cout << "64-bit: ";
        print_binary(reinterpret_cast<uint8_t*>(&n), sizeof(n));
    }
}
