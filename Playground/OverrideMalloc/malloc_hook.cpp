#include <stdexcept>
#include <iostream>

//#define __USE_GNU
#include <dlfcn.h>


#ifndef COLIRU_SANDBOX_MEMORY_LIMIT
#define COLIRU_SANDBOX_MEMORY_LIMIT 1000 * 1000  // 1 MB
#endif


namespace {

std::size_t gSum = 0;

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

#if 0
void free( void* ptr )
{
	typedef void (*FreeFunction)(size_t);
	static FreeFunction real_free = reinterpret_cast<FreeFunction>(dlsym(RTLD_NEXT, "free"));

    real_free(ptr);
}
#endif
