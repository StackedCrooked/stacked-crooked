#include <algorithm>
#include <cstdint>
#include <vector>
#include <iostream>


std::uint8_t cBytes[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
std::size_t cBytesLength = sizeof(cBytes);



std::uint8_t* GetMAC()
{
    static std::uint8_t result[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    return result;
}


static std::vector<std::uint8_t> GetNumbers(unsigned n)
{
    std::vector<std::uint8_t> numbers;
    for (unsigned i = 0; i < n; ++i)
    {
        numbers.push_back(i);
    }

    return numbers;
}


static const std::vector<std::uint8_t> & GetNumbers()
{
    static std::vector<std::uint8_t> fNumbers = GetNumbers(1000);
    std::random_shuffle(fNumbers.begin(), fNumbers.end());
    return fNumbers;
}


std::uint8_t* Random()
{
    static std::uint8_t result[6];
    const std::vector<std::uint8_t> & numbers = GetNumbers();
    result[0] = numbers[0];
    result[1] = numbers[1];
    result[2] = numbers[2];
    result[3] = numbers[3];
    result[4] = numbers[4];
    result[5] = numbers[5];
    return result;
}
