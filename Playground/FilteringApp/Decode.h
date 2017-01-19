#pragma once


#include <cstdint>
#include <cstring>



template<typename T>
T Decode(const uint8_t* data)
{
    T result;
    memcpy(&result, data, sizeof(result));
    return result;
}
