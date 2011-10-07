#include "Pool.h"
#include <stdexcept>


std::vector<Pool*> Pool::sInstances;


Pool & Pool::Get()
{
    if (sInstances.empty())
    {
        throw std::logic_error("There is no pool.");
    }
    return *sInstances.back();
}


Pool::Pool(std::size_t inTotalSize) :
    mData(inTotalSize),
    mUsed(0),
    mFreed(0)
{
    sInstances.push_back(this);
}


Pool::~Pool()
{
    sInstances.pop_back();
}
