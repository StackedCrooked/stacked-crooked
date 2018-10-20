#include "Functor.h"
#include <iostream>


int main(int argc, char** )
{
    Functor* functor = CreateFunctor();
    auto iteration_count = 1000000000;
    for (int i = 0; i != iteration_count; ++i)
    {
        functor->sum(argc, argc);
    }
}
