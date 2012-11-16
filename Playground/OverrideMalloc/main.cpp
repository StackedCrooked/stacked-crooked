#include <iostream>


void *malloc(size_t size);


int main()
{
    unsigned n = 1;
    unsigned sum = 0;
    while (sum < 1000 * 1000)
    {
        malloc(n);
        sum += n;
        n *= 2;
        std::cout << "sum: " << unsigned(sum) << " bytes" << std::endl;
    }
}
