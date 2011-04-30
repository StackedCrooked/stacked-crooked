#include <boost/preprocessor.hpp>
#include <iostream>


#define DEFINE_ENUM(Name, N, Values) \
    enum Name { BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_TO_LIST(N, Values)) };


template<typename T>
struct EnumTypeInfo;

#define VALUE_TO_STRING(Dummy0, Dummy1, Element) \
    BOOST_PP_STRINGIZE(Element)

#define DEFINE_ENUMINFO_SPECIALIZATION(Name, N, _Values, First, Last) \
    template<> struct EnumTypeInfo<Name> \
    { \
        static const char * name() { return #Name; } \
        typedef const Name Values[N]; \
        static Values values; \
        typedef const char * Names[N]; \
        static Names names; \
        static int size() { return N; } \
        static Name first() { return First; } \
        static Name last() { return Last; } \
    };\
    EnumTypeInfo<Name>::Values EnumTypeInfo<Name>::values = { \
        BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_TO_LIST(N, _Values)) \
    }; \
    EnumTypeInfo<Name>::Names EnumTypeInfo<Name>::names = { \
        BOOST_PP_LIST_ENUM(BOOST_PP_LIST_TRANSFORM(VALUE_TO_STRING, N, BOOST_PP_TUPLE_TO_LIST(N, _Values))) \
    };


#define ENUM(Name, N, Values) \
    DEFINE_ENUM(Name, N, Values) \
    DEFINE_ENUMINFO_SPECIALIZATION( \
        Name, \
        N, \
        Values, \
        BOOST_PP_LIST_FIRST(BOOST_PP_TUPLE_TO_LIST(N, Values)), \
        BOOST_PP_LIST_FIRST(BOOST_PP_LIST_REVERSE(BOOST_PP_TUPLE_TO_LIST(N, Values))))


ENUM(RGB, 3, (Red, Green, Blue))


int main()
{
    std::cout << "Name: " << EnumTypeInfo<RGB>::name() << std::endl;
    std::cout << "First: " << EnumTypeInfo<RGB>::first() << std::endl;
    std::cout << "Last: " << EnumTypeInfo<RGB>::last() << std::endl;
    std::cout << "Size: " << EnumTypeInfo<RGB>::size() << std::endl;
    for (int i = 0; i < EnumTypeInfo<RGB>::size(); ++i)
    {
        std::cout << EnumTypeInfo<RGB>::names[i] << "=" << EnumTypeInfo<RGB>::values[i] << std::endl;
    }
    return 0;
}
