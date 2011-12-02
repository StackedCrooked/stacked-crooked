#include <cassert>
#include <cstdlib>
#include <cxxabi.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <typeinfo>


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
struct WithClassName
{
    WithClassName() :
        className_(demangle(typeid(T).name()))
    {
		std::cout << className() << "()\n";
    }

    ~WithClassName()
	{
		std::cout << "~" << className() << "()\n";
	}

    const std::string & className() const
	{
		return className_;
	}

private:
    std::string className_;
};


struct A : public WithClassName<A> { };

struct B : public A, public WithClassName<B> { };


int main()
{
	B b;
}
