#include "GenerateMessage.h"
#include <tuple>
#include <iostream>


GENERATE_MESSAGE(Point,
                 ((int)(x))
                 ((int)(y))
                 ((int)(z)));


template<unsigned N>
struct printer : printer<N - 1 >
{
    template<typename ...Args>
    printer(const std::tuple<Args...> & tup) : printer<N - 1 >(tup)
    {
        enum { index = N - 1 };
        std::cout << std::get<index>(tup) << " ";
    }
};


template<>
struct printer<0>
{
    template<typename T>
    printer(const T &) {}
};


template<typename ...Args>
void print(const std::tuple<Args...> & tup)
{
    printer<sizeof...(Args)>{tup};
}


int main()
{
    Point p(1, 2, 3);
    print(p);
    std::cout << std::endl;
}
