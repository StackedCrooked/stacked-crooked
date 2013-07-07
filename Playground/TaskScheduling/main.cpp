#include "ThreadSupport.h"
#include <iostream>

std::atomic<bool> quit{false};

auto is_quit = []{ return !quit; };


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

    Scheduler s;
    for (int i = 1; i < 1024 * 1024; i = 2 * i)
    {
        s.schedule([=]{ std::cout << "i: " << i << std::endl; }, i);
    }
    sleep(1);
    std::cout << "End of scope!" << std::endl;
}
