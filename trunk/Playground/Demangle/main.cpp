#include <iostream>
#include <stdexcept>
#include <typeinfo>
#include <cxxabi.h>
#include <cstring>
#include <string>
#include <sstream>
#include <cassert>
#include <cstdlib>


std::string demangle(const char * name)
{
    int st;
    char * const p = abi::__cxa_demangle(name, 0, 0, &st);

    if (st != 0)
    {
        switch (st)
        {
            case -1: throw std::runtime_error("A memory allocation failure occurred.");
            case -2: throw std::runtime_error("Not a valid name under the GCC C++ ABI mangling rules.");
            case -3: throw std::runtime_error("One of the arguments is invalid.");
            default: throw std::runtime_error("unexpected demangle status");
        }
    }

    std::string result(p);
    free(p);
    return result;
}


template<class T>
std::string demangle()
{
    return demangle(typeid(T).name());
}


template<class T>
struct WithClassName
{
    WithClassName() :
        className_(demangle<T>())
    {
    }

    inline const std::string & className() const { return className_; }

private:
    std::string className_;
};


template<class T>
struct Policy : public WithClassName<T>
{
    Policy() { std::cout << this->className() << "()" << std::endl; }

    ~Policy() { std::cout << "~" << this->className() << "()" << std::endl; }
};


struct A : public Policy<A> { };
struct B : public A,
           public Policy<B> { };

int main()
{
    B b;
}
