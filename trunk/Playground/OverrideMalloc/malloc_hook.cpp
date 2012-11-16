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

std::size_t gNumAllocations = 0;
std::size_t gNumRejectedAllocations = 0;
std::size_t gNumUnmanagedFrees = 0;
std::size_t gNumManagedFrees = 0;


#define PRINT_COUNTER(name) fprintf(stderr, #name ": %lu\n", name);

struct info
{
    info()
    {
        fprintf(stderr, "Initial alloc: %lu bytes.\n", gSum);
    }
    ~info()
    {
        PRINT_COUNTER(gSum);
        PRINT_COUNTER(gNumAllocations);
        PRINT_COUNTER(gNumRejectedAllocations);
        PRINT_COUNTER(gNumUnmanagedFrees);
        PRINT_COUNTER(gNumManagedFrees);
    }
} gInfo;


class ScopedToggle
{
public:
    ScopedToggle(bool & b) : b(b) { b = !b; }

    ~ScopedToggle() { b = !b; }

private:
    ScopedToggle(const ScopedToggle&) = delete;
    ScopedToggle& operator=(const ScopedToggle&) = delete;
    bool & b;
};


} // anonymous namespace


void *malloc(size_t size)
{
    typedef void* (*AllocFunction)(size_t);
    static AllocFunction real_malloc = reinterpret_cast<decltype(real_malloc)>(dlsym(RTLD_NEXT, "malloc"));

    static bool fInsideMalloc = false;
    if (fInsideMalloc)
    {
        return real_malloc(size);
    }
    ScopedToggle toggle(fInsideMalloc);

    gSum += size;
    if (gSum > COLIRU_SANDBOX_MEMORY_LIMIT)
    {
        gNumRejectedAllocations++;
        return 0;
    }
    else
    {
        gNumAllocations++;
        auto result = real_malloc(size);
        gAllocations.insert(std::make_pair(result, size));
        return result;
    }
}

void free( void* ptr )
{
    typedef void (*FreeFunction)(void*);
    static FreeFunction real_free = reinterpret_cast<FreeFunction>(dlsym(RTLD_NEXT, "free"));

    static bool fInsideFree = false;
    if (fInsideFree)
    {
        real_free(ptr);
        return;
    }
    ScopedToggle toggle(fInsideFree);

    auto it = gAllocations.find(ptr);
    if (it != gAllocations.end())
    {
        auto amount = it->second;
        gSum -= amount;
        it->second = 0;
        gNumManagedFrees++;
    }
    else
    {
        gNumUnmanagedFrees++;
    }
    real_free(ptr);
}
