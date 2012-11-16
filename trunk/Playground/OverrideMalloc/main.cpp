#include <iostream>
#include <string>
#include <stdexcept>
#include <stdlib.h>


int main()
{
    auto n = 1ull;
    auto sum = 0ull;
    while (sum < 10 * 1000 * 1000)
    {
        auto ptr = malloc(n);
        std::cout << "ptr: " << ptr << std::endl;
        if (!ptr)
        {
            throw std::runtime_error("Failed to allocate " + std::to_string(n) + " more bytes.");
        }
        sum += n;
        n *= 2;
        std::cout << "sum: " << unsigned(sum) << " bytes" << std::endl;
    }
}
