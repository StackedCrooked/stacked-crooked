#include "StrongTypedef.h"
#include <iostream>


FUTILE_STRONG_TYPEDEF(int, Width)
FUTILE_STRONG_TYPEDEF(int, Height)


int Area(Width w, Height h)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return w * h;
}

template<typename T>
void Test(T);



int main()
{
    std::cout << Area(Width(3), Height(4)) << std::endl;


#if 0
    // Invoke compiler error to see the type name
    Test(Width(3));
    Test(Height(3));
#endif 
}
