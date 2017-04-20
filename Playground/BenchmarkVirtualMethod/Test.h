#pragma once


struct Test 
{
    int inline_foo() { return 1; }
    int foo();
    virtual int vfoo();
};

Test& get_test();


