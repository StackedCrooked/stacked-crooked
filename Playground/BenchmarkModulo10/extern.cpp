#include <cstdint>
#include <cstdlib>


volatile const int64_t volatile_zero = 0;
volatile int64_t volatile_sink = 0;


uint64_t* allocate_items(int64_t num_items)
{
    return static_cast<uint64_t*>(malloc(num_items * sizeof(int64_t)));
}
