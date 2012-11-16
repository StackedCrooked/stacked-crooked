#include <iostream>
#include <stdexcept>
#include <stdlib.h>


int main()
{
    auto n = 1llu;
    auto sum = 0llu;
    while (sum < 11)
    {
        void * ptr = malloc(n);
        std::cout << "ptr: " << ptr << std::endl;
        if (!ptr)
        {
            throw std::runtime_error("Failed to allocate " + std::to_string(n) + " more bytes.");
        }
        sum += n;
        std::cout << "sum: " << unsigned(sum) << " bytes" << std::endl;
    }
}
