#include "OOP.h"
#include "Overloading.h"
#include "Policy.h"


using namespace Policy;


int main()
{
    {
        RecursiveMutex mutex;
        mutex.lock();
        mutex.unlock();
    }

    {
        Mutex mutex;
        mutex.lock();
        mutex.unlock();
    }

    // Doesn't compile
    //{
    //    Mutex<Recursive::Yes, Spin::Yes> mutex;
    //    mutex.lock();
    //    mutex.unlock();
    //}

    {
        SpinMutex mutex;
        mutex.lock();
        mutex.unlock();
    }
    return 0;
}
