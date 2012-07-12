#include "StrongTypedef.h"
#include <iostream>


FUTILE_STRONG_TYPEDEF(int, Width)
FUTILE_STRONG_TYPEDEF(int, Height)


int Area(Width w, Height h)
{
    return w * h;
}


int main()
{
    std::cout << Area(Width(3), Height(4)) << std::endl;
}
