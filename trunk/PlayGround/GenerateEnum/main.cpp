#include <boost/preprocessor.hpp>
#include <iostream>


#define DEFINE_ENUM(Tag, Size, Enumerator) \
    enum Tag { BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_TO_LIST(Size, Enumerator)) };


template<typename T>
struct EnumInfo;

#define VALUE_TO_STRING(Dummy0, Dummy1, Element) \
    BOOST_PP_STRINGIZE(Element)

#define DEFINE_ENUMTYPEINFO_SPECIALIZATION(Tag, Size, Enumerator, First, Last) \
    template<> struct EnumInfo<Tag> \
    { \
        static const char * name() { return #Tag; } \
        typedef const Tag Values[Size]; \
        static Values values; \
        typedef const char * Names[Size]; \
        static Names names; \
        static int size() { return Size; } \
        static Tag first() { return First; } \
        static Tag last() { return Last; } \
    };\
    EnumInfo<Tag>::Values EnumInfo<Tag>::values = { \
        BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_TO_LIST(Size, Enumerator)) \
    }; \
    EnumInfo<Tag>::Names EnumInfo<Tag>::names = { \
        BOOST_PP_LIST_ENUM(BOOST_PP_LIST_TRANSFORM(VALUE_TO_STRING, Size, BOOST_PP_TUPLE_TO_LIST(Size, Enumerator))) \
    };


template<class Enum, Enum _Enumerator>
struct EnumeratorInfo;


#define DEFINE_ENUMVALUEINFO_SPECIALIZATION(Dummy, Enum, Enumerator) \
    template<> struct EnumeratorInfo<Enum, Enumerator> { \
        static const char * name() { return #Enumerator; } \
        static Enum value() { return Enumerator; } \
    };


#define ENUM(Tag, Size, Values) \
    DEFINE_ENUM(Tag, Size, Values) \
    DEFINE_ENUMTYPEINFO_SPECIALIZATION( \
        Tag, \
        Size, \
        Values, \
        BOOST_PP_LIST_FIRST(BOOST_PP_TUPLE_TO_LIST(Size, Values)), \
        BOOST_PP_LIST_FIRST(BOOST_PP_LIST_REVERSE(BOOST_PP_TUPLE_TO_LIST(Size, Values)))) \
    BOOST_PP_LIST_FOR_EACH( \
        DEFINE_ENUMVALUEINFO_SPECIALIZATION, \
        Tag, \
        BOOST_PP_TUPLE_TO_LIST(Size, Values))




ENUM(RGB, 3, (Red, Green, Blue))


int main()
{
    std::cout << "Tag: " << EnumInfo<RGB>::name() << std::endl;
    std::cout << "First: " << EnumInfo<RGB>::first() << std::endl;
    std::cout << "Last: " << EnumInfo<RGB>::last() << std::endl;
    std::cout << "Size: " << EnumInfo<RGB>::size() << std::endl;
    for (int i = 0; i < EnumInfo<RGB>::size(); ++i)
    {
        std::cout << EnumInfo<RGB>::names[i] << "=" << EnumInfo<RGB>::values[i] << std::endl;
    }

    std::cout << EnumeratorInfo<RGB, Red>::name() << std::endl;
    std::cout << EnumeratorInfo<RGB, Green>::name() << std::endl;
    std::cout << EnumeratorInfo<RGB, Blue>::name() << std::endl;


    return 0;
}
