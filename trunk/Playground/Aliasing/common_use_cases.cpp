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
 * buggy_decode tries to create a c++ object from a network data packet.
 *
 * This implementation is buggy for the following reasons:
 * (1) According to the c++ type system no object has ever been constructed in
 *     that memory location. We are not allowed to alias a non-existing
 *     object. (*)
 * (2) The pointer address of the network data may not match the native
 *     aligment of the target type. Misaligned objects can lead to
 *     performance overhead (Intel) or to process termination (ARM).
 *     
 * (*) Technically we could use placement-new to construct an object that
 *     overlays the memory segment. However, that doesn't help us much
 *     futher because it does nothing to prevent the potential alignment
 *     problems. Furthermore the constructed object must be initialized
 *     before we can legally read from its contents. "Initialization" implies
 *     overwriting the network packet data. This obviously defeats our purpose.
 */
template<typename T>
inline const T & buggy_decode(const uint8_t * bytes)
{
    static_assert(std::is_pod<T>::value, "T must meet POD requirements.");
    return *reinterpret_cast<const T *>(bytes);
}

/**
 * correct_decode creates a c++ pod object from a network data packet.
 *
 * It copies the data into a stack-allocated object of type T.
 * Allocation on the stack ensures that T has native alignment.
 * In order to creaate a binary copy of the network data over our object we
 * need to obtain binary access to the target object. This can be achieved
 * with one `reinterpret_cast` or by chaining two `static_cast`: 
 *   (1) reinterpret_cast<char*>(&t);
 *   (2) static_cast<char*>(static_cast<void*>(&t));
 */
template<typename T>
T correct_decode(const uint8_t * bytes)
{
    T t;
    std::copy(bytes, bytes + sizeof(T), static_cast<char*>(static_cast<void*>(&t)));
    return t;
}
    

void common_use_cases()
{
    example_checksum16();
}