#include <stdexcept>

#define __USE_GNU
#include <dlfcn.h>


typedef void* (*AllocFunction)(size_t);


void *malloc(size_t size)
{
    static AllocFunction real_malloc = reinterpret_cast<decltype(real_malloc)>(dlsym(RTLD_NEXT, "malloc"));

    static std::size_t fSum = 0;
    fSum += size;
    if (fSum > 1000)
    {
        throw std::bad_alloc();
    }
    return real_malloc(size);
}
