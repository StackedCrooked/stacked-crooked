#include "Test.h"


struct DerivedTest : Test
{
    int vfoo() { return 2; }
};


Test& get_test()
{
    return *new DerivedTest;
}
