#include "Utils.h"
#include <cxxabi.h>


const char* Demangle(const char* mangled_name)
{
    int st;
    auto demangled = abi::__cxa_demangle(mangled_name, 0, 0, &st);
    return st == 0 ? demangled : mangled_name;
}
