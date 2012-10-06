#include <iostream>
#include <vector>
#include <type_traits>

#define BAD_CODE  0
#define GOOD_CODE 1


#if 0


const uint32_t * csrc = reinterpret_cast<const uint32_t *>(this + sizeof(RTPHeader));




#endif


void example_checksum16()
{
    
    
}

/**
 * buggy_decode aims to overlay an C++ type-view over a segment of network data.
 *
 * This implementation is buggy for the following reasons:
 * (1) According to the C++ type system no object has ever been constructed in
 *     that memory location. We are not allowed to alias a non-existing
 *     object. (*)
 * (2) The pointer address of the network data may not match the native
 *     aligment of the target type. Misaligned objects can lead to
 *     performance overhead (Intel) or to process termination (ARM).
 *     
 *  (*) We could use placement-new to construct an object that overlays the
 *      memory segment. However we would then be required to "initialize"
 *      the object before we can legally read from it. This would mean erasing
 *      the network data that we are trying to get access to. On the other
 *      hand, accessing the uninitialized object is akin to accessing any other
 *      uninitialized data from the C++ standard point-of-view and would result
 *      in Undefined Behavior.
 *      Also note that placement-new does not prevent the problems caused
 *      by misalignment.
 */
template<typename T>
inline const T & buggy_decode(const uint8_t * bytes)
{
    static_assert(std::is_pod<T>::value, "T must meet POD requirements.");
    return *reinterpret_cast<const T *>(bytes);
}

/**
 * correct_decode creates a C++ object from network data.
 * It alloates an object of type T on the stack. This ensures corrrect alignment.
 * We proceed by copying the network data bytewise into our object.
 */
T correct_decode(const uint8_t * bytes)
{
    T t;
    std::copy(bytes, bytes + sizeof(T), reinterpret_cast<char*>(&t));
    return t;
}
    

void common_use_cases()
{
    example_checksum16();
}
