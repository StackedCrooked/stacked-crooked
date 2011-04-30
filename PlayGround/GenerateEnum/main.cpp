#include <boost/lexical_cast.hpp>
#include <boost/preprocessor.hpp>
#include <stdexcept>
#include <iostream>


#define DEFINE_ENUM(Tag, Size, Enumerator) \
    enum Tag { BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_TO_LIST(Size, Enumerator)) };

/**
 * EnumInfo<EnumType> provides useful meta data beloning to an enum type:
 *   - name() return the enumeration's tag name.
 *   - values is an array containing all enumerator values.
 *   - names is an array containing all enumerator names.
 *   - size() returns the number of enumerators.
 *   - first() returns the first enumerator.
 *   - last() returns the last enumerator.
 *   - FromString(const std::string &) returns the enumerator value for the given string value.
 *       An exception is thrown if the string was not found in the list of enumerator names.
 *   - ToString(const std::string &) returns the enumerator name for a given enumerator value.
 *       A std::runtime_error is thrown if the enumerator value was not found.
 */
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
        static Tag FromString(const std::string & inName) { \
            for (std::size_t idx = 0; idx < size(); ++idx) { \
                if (names[idx] == inName) return values[idx]; \
            } \
            throw std::runtime_error("Color name not found: " + inName); \
        } \
        static const char * ToString(Tag inValue) { \
            for (std::size_t idx = 0; idx < size(); ++idx) { \
                if (values[idx] == inValue) return names[idx]; \
            } \
            throw std::runtime_error("Color index not found: " + boost::lexical_cast<std::string>(inValue)); \
        } \
    };\
    EnumInfo<Tag>::Values EnumInfo<Tag>::values = { \
        BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_TO_LIST(Size, Enumerator)) \
    }; \
    EnumInfo<Tag>::Names EnumInfo<Tag>::names = { \
        BOOST_PP_LIST_ENUM(BOOST_PP_LIST_TRANSFORM(VALUE_TO_STRING, Size, BOOST_PP_TUPLE_TO_LIST(Size, Enumerator))) \
    };


/**
 * EnumeratorInfo<EnumType, Enumerator> provides useful metadata for the requested Enumerator value.
 *   - name() returns the Enumerator name
 *   - value() returns the Enumerator value
 */
template<class TEnum, TEnum TEnumerator>
struct EnumeratorInfo;


#define DEFINE_ENUMVALUEINFO_SPECIALIZATION(Dummy, Enum, Enumerator) \
    template<> struct EnumeratorInfo<Enum, Enumerator> { \
        typedef Enum EnumType; \
        static const char * name() { return #Enumerator; } \
        static Enum value() { return Enumerator; } \
    };


//
// Macro for defining a new enum.
//
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


//
// Define the Color enum
//
ENUM(Color, 7, (Red, Orange, Yellow, Green, Blue, Indigo, Violet));


//
// Define the Note enum
//
ENUM(Note, 7, (Do, Re, Mi, Fa, Sol, La, Si))


template<class Enum>
void TestEnum()
{
    std::cout << "Tag: " << EnumInfo<Enum>::name() << std::endl;
    std::cout << "First: " << EnumInfo<Enum>::first() << std::endl;
    std::cout << "Last: " << EnumInfo<Enum>::last() << std::endl;
    std::cout << "Size: " << EnumInfo<Enum>::size() << std::endl;
    for (int i = 0; i < EnumInfo<Enum>::size(); ++i)
    {
        std::cout << EnumInfo<Enum>::names[i] << "=" << EnumInfo<Enum>::values[i] << std::endl;
    }

    try
    {
        {
            std::cout << "Please input a color name. (Possible values are ";
            for (Enum i = EnumInfo<Enum>::first(); i <= EnumInfo<Enum>::last(); i = static_cast<Enum>(static_cast<int>(i) + 1))
            {
                if (i != EnumInfo<Enum>::first())
                {
                    std::cout << ", ";
                }
                std::cout << EnumInfo<Enum>::names[i];
            }
            std::cout << "): ";
            std::string colorName;
            std::cin >> colorName;

            Enum enumValue = EnumInfo<Enum>::FromString(colorName);
            std::cout << "This color has the value: " << enumValue << "." << std::endl
                      << "And it's name is: "         << EnumInfo<Enum>::ToString(enumValue)
                      << std::endl << std::flush;

        }

        {
            std::cout << "Please input a enumerator value. (Possible values are ";
            for (Enum i = EnumInfo<Enum>::first(); i <= EnumInfo<Enum>::last(); i = static_cast<Enum>(static_cast<int>(i) + 1))
            {
                if (i != EnumInfo<Enum>::first())
                {
                    std::cout << ", ";
                }
                std::cout << i;
            }
            std::cout << "): ";
            int enumerator;
            std::cin >> enumerator;
            if (enumerator >= EnumInfo<Enum>::first() && enumerator <= EnumInfo<Enum>::last())
            {
                Enum enumValue = static_cast<Enum>(enumerator);
                std::cout << "The corresponding color name is: " << EnumInfo<Enum>::ToString(enumValue) << std::endl << std::flush;
            }

        }
        std::cout << std::endl << std::flush;
    }
    catch (const std::exception & exc)
    {
        std::cerr << exc.what() << std::endl;
    }
}

template<class TEnumType, TEnumType Enumerator>
void TestEnumerator()
{
    typedef typename EnumeratorInfo<TEnumType, Enumerator>::EnumType Enum;
    std::cout << "The enumerator value is: " << EnumeratorInfo<Enum, Enumerator>::value() << "." << std::endl;
    std::cout << "The enumerator name is: " << EnumeratorInfo<Enum, Enumerator>::name() << "." << std::endl;
    std::cout << "The enumerator belongs to the " << EnumInfo<Enum>::name() << " enum." << std::endl;
    std::cout << std::endl << std::flush;
}


int main()
{
    TestEnum<Note>();
    TestEnumerator<Note, Sol>();
    TestEnumerator<Note, Si>();

    TestEnum<Color>();
    TestEnumerator<Color, Yellow>();
    TestEnumerator<Color, Orange>();
    return 0;
}

