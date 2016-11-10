#ifndef UTILS_H
#define UTILS_H


#include <cstring>
#include <typeinfo>


template<typename T>
inline T Decode(const uint8_t* data)
{
    auto result = T();
    memcpy(&result, data, sizeof(result));
    return result;
}

const char* Demangle(char const * mangled_name);


template<typename T>
const char* GetTypeName()
{
    return Demangle(typeid(T).name());
}


#endif // UTILS_H
