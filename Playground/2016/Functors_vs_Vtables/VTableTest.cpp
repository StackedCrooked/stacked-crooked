#include "VTable.h"
#include <iostream>


int main(int argc, char**)
{
    VTable* vtable = CreateVTable();
    auto iteration_count = 1000000000;
    for (int i = 0; i != iteration_count; ++i)
    {
        vtable->sum(argc, argc);
    }
}
