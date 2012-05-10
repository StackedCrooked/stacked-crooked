#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>


extern std::uint8_t cBytes[];
extern std::size_t cBytesLength;


struct A
{
    uint8_t bytes[6];
};


inline bool operator<(const A & lhs, const A & rhs)
{
    return std::memcmp(lhs.bytes, rhs.bytes, sizeof(lhs.bytes)) < 0;
}

struct B
{
    std::uint32_t first;
    std::uint16_t second;
};


inline bool operator<(const B & lhs, const B & rhs)
{
    return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
}


struct C
{
    std::uint16_t first;
    std::uint32_t second;
};


inline bool operator<(const C & lhs, const C & rhs)
{
    return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
}


int main()
{
    std::cout << cBytesLength << std::endl;
}
