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
 * First we ensure that our c++ has native alignment by allocating it
 * at a local variable on the stack. 
 * Next we obtain a byte pointer to the memory segment occupied by our
 * object. This form of aliasing is explicitly allowed by the c++ standard.
 *
 * The byte pointer can be obtained in two ways:
 *   (1) reinterpret_cast<char*>(&t);
 *   (2) static_cast<char*>(static_cast<void*>(&t));
 *
 * We must return the object by value because it is a local variable. In theory
 * this means an extra copy. In practice this copy is fully optimized away by
 * thanks to the Named-Return-Value-Optimization.
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
