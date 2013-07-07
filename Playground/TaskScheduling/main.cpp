#include "ThreadSupport.h"
#include <iostream>

int main()
{
    using namespace ThreadSupport;

    Async([]{ std::cout << "ABC" << std::endl; }).get();
}
