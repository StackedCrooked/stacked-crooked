#include "Threading.h"
#include <iostream>


using namespace Threading;


struct Point
{
    Point(int x_, int y_) : x(x_), y(y_) {}
    int x;
    int y;
};

int main()
{
    FastMutex theFastMutex;
    FastMutex::ScopedLock theScopedLock(theFastMutex);

    typedef ThreadSafe<Point> TSPoint;
    TSPoint tsPoint(new Point(3, 4));
    ScopedAccessor<Point> pointAccess(tsPoint);
    std::cout << "x: " << pointAccess.get().x << std::endl;
    std::cout << "y: " << pointAccess.get().y << std::endl;

    std::cout << "x: " << pointAccess->x << std::endl;
    std::cout << "y: " << pointAccess->y << std::endl;


    std::cout << "Everything went better than expected." << std::endl << std::flush;
    return 0;
}
