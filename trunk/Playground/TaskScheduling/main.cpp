#include "ThreadSupport.h"
#include <iostream>

int main()
{
    using namespace ThreadSupport;

    Async([]{ std::cout << "ABC" << std::endl; }).get();
    std::future<int> fi = Async([](int a, int b){
        return a + b;
    }, 3, 4);
    std::cout << fi.get() << std::endl;

}
