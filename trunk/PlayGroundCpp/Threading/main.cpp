#include "Threading.h"
#include <iostream>


using Threading::ThreadSafe;


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

    ThreadSafe<Point, Threading::PosixMutex> mPoint;
};

int main()
{
    Worker worker;
    worker = Worker();
    Worker worker2 = worker;

    std::cout << "Everything went better than expected." << std::endl;
    return 0;
}
