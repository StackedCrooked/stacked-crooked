#include <boost/preprocessor.hpp>
#include <iostream>


#define DEFINE_ENUM(Name, N, Values) \
    enum Name { BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_TO_LIST(N, Values)) };


template<typename T>
struct EnumInfo
{
};

#define DEFINE_ENUMINFO_SPECIALIZATION(Name, N, First, Last) \
    template<> struct EnumInfo<Name> \
    { \
        static const char * name() { return #Name; } \
        static int size() { return N; } \
        static Name first() { return First; } \
        static Name last() { return Last; } \
    };


#define ENUM(Name, N, Values) \
    DEFINE_ENUM(Name, N, Values) \
    DEFINE_ENUMINFO_SPECIALIZATION( \
        Name, \
        N, \
        BOOST_PP_LIST_FIRST(BOOST_PP_TUPLE_TO_LIST(N, Values)), \
        BOOST_PP_LIST_FIRST(BOOST_PP_LIST_REVERSE(BOOST_PP_TUPLE_TO_LIST(N, Values))))


ENUM(RGB, 3, (Red, Green, Blue))


int main()
{
    std::cout << "Name: " << EnumInfo<RGB>::name() << std::endl;
    std::cout << "First: " << EnumInfo<RGB>::first() << std::endl;
    std::cout << "Last: " << EnumInfo<RGB>::last() << std::endl;
    std::cout << "Size: " << EnumInfo<RGB>::size() << std::endl;
    return 0;
}
