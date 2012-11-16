#include <stdexcept>
#include <iostream>
#include <unordered_map>

//#define __USE_GNU
#include <dlfcn.h>


#ifndef COLIRU_SANDBOX_MEMORY_LIMIT
#define COLIRU_SANDBOX_MEMORY_LIMIT 1000 * 1000  // 1 MB
#endif


namespace {

std::size_t gSum = 0;
std::unordered_map<void*, std::size_t> gAllocations;

}


void *malloc(size_t size)
{
	typedef void* (*AllocFunction)(size_t);
	static AllocFunction real_malloc = reinterpret_cast<decltype(real_malloc)>(dlsym(RTLD_NEXT, "malloc"));

	gSum += size;
	if (gSum > COLIRU_SANDBOX_MEMORY_LIMIT)
	{
		return 0;
	}
	else
	{
		return real_malloc(size);
	}
}

void free( void* ptr )
{
	typedef void (*FreeFunction)(void*);
	static FreeFunction real_free = reinterpret_cast<FreeFunction>(dlsym(RTLD_NEXT, "free"));

    auto it = gAllocations.find(ptr);
    if (it == gAllocations.end())
    {
        fprintf(stderr, "free: unkown pointer: %p", ptr);
        return;
    }

    auto amount = it->second;
    if (amount > gSum)
    {
        fprintf(stderr, "Amount that is to be freed is less than sum. Amount=%lu, sum=%lu", amount, gSum);
        return;
    }

    gSum -= amount;
    gAllocations.erase(it);
    real_free(ptr);
}
