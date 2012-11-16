#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stdexcept>


#ifndef COLIRU_SANDBOX_MEMORY_LIMIT
#error Build command should specify -DCOLIRU_SANDBOX_MEMORY_LIMIT=...
#endif


#define TEST_MALLOC 0
#define TEST_NEW    1
#define TEST_CASE   TEST_NEW

int main()
{
#if TEST_CASE == TEST_MALLOC
    assert(malloc(COLIRU_SANDBOX_MEMORY_LIMIT - 1));
    assert(malloc(1));
    assert(!malloc(1));
#elif TEST_CASE == TEST_NEW
    new char[COLIRU_SANDBOX_MEMORY_LIMIT - 1]; // OK
    new char[1]; // OK
    try
    {
        new char[1]; // Error!
        assert(false);
    }
    catch (std::bad_alloc& exc)
    {
    }
#endif
}
