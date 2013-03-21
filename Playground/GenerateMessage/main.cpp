#include "GenerateMessage.h"
#include <tuple>
#include <iostream>


GENERATE_MESSAGE(Point,
                 ( (int)(x) )
                 ( (int)(y) ));


int main()
{
    Point p(1, 2);
    std::cout << "(" << p.get_x() << ", " << p.get_y() << ")" << std::endl;
}

