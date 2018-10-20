#pragma once


struct Test 
{
    int foo();
    virtual int vfoo();
};


Test& get_test();
