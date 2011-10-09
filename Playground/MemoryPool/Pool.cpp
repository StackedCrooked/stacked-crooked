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


static void Free(char * buffer)
{
    free(reinterpret_cast<void*>(buffer));
}


Pool::Pool(std::size_t inSize) :
    mData(Free),
    mSize(inSize),
    mUsed(0),
    mFreed(0)
{
    mData.reset(reinterpret_cast<char*>(malloc(inSize)));
    sInstances.push_back(this);
}


Pool::~Pool()
{
    sInstances.pop_back();
}
