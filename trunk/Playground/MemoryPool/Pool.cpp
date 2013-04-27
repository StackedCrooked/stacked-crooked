#include "Pool.h"
#include <cstdlib>
#include <cstring>
#include <stdexcept>


namespace { // anonymous


typedef std::vector<Pool*> Instances;


Instances & GetPoolInstances()
{
    static Instances fInstances;
    return fInstances;
}


unsigned char * Alloc(std::size_t inSize)
{
    return reinterpret_cast<unsigned char*>(malloc(inSize));
}


void Free(unsigned char * buffer)
{
    free(reinterpret_cast<void*>(buffer));
}


} // anonymous namespace


Pool & Pool::Get()
{
    if (GetPoolInstances().empty())
    {
        throw std::logic_error("There is no pool.");
    }
    return *GetPoolInstances().back();
}


Pool::Pool(std::size_t inSize) :
    mData(inSize),
    mSize(inSize),
    mUsed(0),
    mFreed(0)
{
    GetPoolInstances().push_back(this);
}


Pool::~Pool()
{
    // Release the memory occupied by the data object.
    Data().swap(mData);

    GetPoolInstances().pop_back();
}
