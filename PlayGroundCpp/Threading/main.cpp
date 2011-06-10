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
    ScopedAccessor<Point> locker(tsPoint);
    std::cout << "x: " << locker.get().x << std::endl;
    std::cout << "y: " << locker.get().y << std::endl;


    std::cout << "Everything went better than expected." << std::endl << std::flush;
    return 0;
}
