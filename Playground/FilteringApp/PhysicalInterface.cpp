#include "PhysicalInterface.h"







void PhysicalInterface::pop_now()
{
    for (BBInterface* i : mActiveBBInterfaces)
    {
        i->pop_now();
    }

    mActiveBBInterfaces.clear();
}
