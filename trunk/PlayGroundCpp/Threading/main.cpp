#include "Threading.h"
#include <iostream>

int main()
{
    Threading::FastMutex theFastMutex;
    Threading::FastMutex::ScopedLock theScopedLock(theFastMutex);

    std::cout << "Everything went better than expected." << std::endl << std::flush;
    return 0;
}
