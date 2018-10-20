#ifndef UTILS_H
#define UTILS_H


#include <chrono>
#include <typeinfo>


using Clock = std::chrono::steady_clock;


const char* Demangle(char const * mangled_name);


template<typename T>
const char* GetTypeName()
{
    return Demangle(typeid(T).name());
}


#endif // UTILS_H
