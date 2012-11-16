#include <iostream>
#include <stdexcept>
#include <stdlib.h>


int main()
{
    unsigned n = 1;
    unsigned sum = 0;
    while (sum < 10 * 1000 * 1000)
    {
        void * ptr = malloc(n);
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
