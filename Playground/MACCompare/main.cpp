#include <cstddef>
#include <cstdint>
#include <iostream>


extern std::uint8_t cBytes[];
extern std::size_t cBytesLength;


int main()
{
    std::cout << cBytesLength << std::endl;
}
