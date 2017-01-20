#pragma once


#include <cstdint>
#include <cstring>



template<typename T>
T Decode(const uint8_t* data)
{
    if (alignof(T) == alignof(uint8_t)) // compile-time branch
    {
        return *reinterpret_cast<const T*>(data);
    }
    else
    {
        T result;
        memcpy(&result, data, sizeof(result));
        return result;
    }
}
