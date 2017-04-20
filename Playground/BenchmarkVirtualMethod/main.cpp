#include "Test.h"
#include <chrono>
#include <iostream>


enum { num_iterations = 1000000000 };


void test_foo()
{
    Test& t = get_test();
    auto start_time = std::chrono::steady_clock::now();
    auto sum = 0ul;
    for (auto i = 0u; i != num_iterations; ++i)
    {
        sum += t.foo();
    }
    auto elapsed_ns = std::chrono::nanoseconds(std::chrono::steady_clock::now() - start_time).count();
    std::cout << __FUNCTION__ << ": elapsed_ns=" << elapsed_ns << " ns/iteration=" << 1.0 * elapsed_ns / num_iterations << " sum=" << sum << std::endl;
}


void test_vfoo()
{
    Test& t = get_test();
    auto start_time = std::chrono::steady_clock::now();
    auto sum = 0ul;
    for (auto i = 0u; i != num_iterations; ++i)
    {
        sum += t.vfoo();
    }
    auto elapsed_ns = std::chrono::nanoseconds(std::chrono::steady_clock::now() - start_time).count();
    std::cout << __FUNCTION__ << ": elapsed_ns=" << elapsed_ns << " ns/iteration=" << 1.0 * elapsed_ns / num_iterations << " sum=" << sum << std::endl;
}


int main()
{
    test_foo();
    test_foo();
    test_vfoo();
    test_vfoo();
    test_foo();
    test_foo();
    test_vfoo();
    test_vfoo();
}
