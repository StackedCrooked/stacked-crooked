#include "Threading.h"
#include <iostream>


using namespace Threading;


struct Point
{
    Point(int x_, int y_) : x(x_), y(y_) {}
    int x;
    int y;
};

struct Worker
{
    Worker() :
        mPoint(new Point(3, 4))
    {
    }

    ThreadSafe<Point> mPoint;
};

int main()
{
    Worker w0;
    Worker w1 = w0;
    Worker w2;
    w2 = w1;
    return 0;
}
