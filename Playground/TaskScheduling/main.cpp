#include "ThreadSupport.h"
#include <iostream>

int main()
{
    using namespace ThreadSupport;

    Async([]{ std::cout << "ABC" << std::endl; }).get();
    auto sum = [](int a, int b){ return a + b; };
    std::cout << Async(sum, 3, 4).get() << std::endl;


    int n = 42;
    auto increment = [](int& n) { n++; };

    Async(increment, std::ref(n)).get();
    std::cout << n << std::endl;

}
