#include <boost/lexical_cast.hpp>
#include <boost/preprocessor.hpp>
#include <stdexcept>
#include <iostream>


/**
 * EnumInfo<EnumType> provides useful meta data beloning to an enum type.
 *
 * Details:
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


/**
 * EnumeratorInfo<EnumType, Enumerator> provides useful metadata for the requested Enumerator value.
 *
 * Details:
 *   - name() returns the Enumerator name
 *   - value() returns the Enumerator value
 */
template<class TEnum, TEnum TEnumerator>
struct EnumeratorInfo;


//
// Macros for internal use
//
#define DEFINE_ENUM(Tag, Size, Enumerator) \
    enum Tag { BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_TO_LIST(Size, Enumerator)) };

#define VALUE_TO_STRING(Dummy0, Dummy1, Element) \
    BOOST_PP_STRINGIZE(Element)

#define DEFINE_ENUMINFO_SPECIALIZATION(Tag, Size, Enumerator, First, Last) \
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
            throw std::runtime_error("Invalid enumerator name: " + inName); \
        } \
        static const char * ToString(Tag inValue) { \
            for (std::size_t idx = 0; idx < size(); ++idx) { \
                if (values[idx] == inValue) return names[idx]; \
            } \
            throw std::runtime_error("Invalid enumerator value: " + boost::lexical_cast<std::string>(inValue)); \
        } \
    };\
    EnumInfo<Tag>::Values EnumInfo<Tag>::values = { \
        BOOST_PP_LIST_ENUM(BOOST_PP_TUPLE_TO_LIST(Size, Enumerator)) \
    }; \
    EnumInfo<Tag>::Names EnumInfo<Tag>::names = { \
        BOOST_PP_LIST_ENUM(BOOST_PP_LIST_TRANSFORM(VALUE_TO_STRING, Size, BOOST_PP_TUPLE_TO_LIST(Size, Enumerator))) \
    };


#define DEFINE_ENUMERATORINFO_SPECIALIZATION(Dummy, Enum, Enumerator) \
    template<> struct EnumeratorInfo<Enum, Enumerator> { \
        typedef Enum EnumType; \
        static const char * name() { return #Enumerator; } \
        static Enum value() { return Enumerator; } \
    };


/**
 * The ENUM macro allows you to define a new enum along with accompanying
 * code for parsing and serializing the enumerator values.
 *
 * Usage example:
 *
 *    ENUM(HTTPRequestMethod, 9, (HEAD, GET, POST, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH))
 *
 *
 */
#define ENUM(Tag, Size, Values) \
    DEFINE_ENUM(Tag, Size, Values) \
    DEFINE_ENUMINFO_SPECIALIZATION( \
        Tag, \
        Size, \
        Values, \
        BOOST_PP_LIST_FIRST(BOOST_PP_TUPLE_TO_LIST(Size, Values)), \
        BOOST_PP_LIST_FIRST(BOOST_PP_LIST_REVERSE(BOOST_PP_TUPLE_TO_LIST(Size, Values)))) \
    BOOST_PP_LIST_FOR_EACH( \
        DEFINE_ENUMERATORINFO_SPECIALIZATION, \
        Tag, \
        BOOST_PP_TUPLE_TO_LIST(Size, Values))


//
// Tests
//


// Define the Color enum
ENUM(Color, 7, (Red, Orange, Yellow, Green, Blue, Indigo, Violet))

// Define the Note enum
ENUM(Note, 7, (Do, Re, Mi, Fa, Sol, La, Si))

// Define the HTTPRequestMethod enum
ENUM(HTTPRequestMethod, 9, (HEAD, GET, POST, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH))


void PauseConsole(const std::string & inMessage)
{
    std::string message = "\"" + inMessage + "\"";
    #ifdef _WIN32
    std::string command = "pause ";
    #else
    std::string command = "read -p ";
    #endif
    system((command + message).c_str());
}


template<class Enum>
void TestEnum()
{
    std::cout << "=== Enum Test ===" << std::endl;
    try
    {
        {
            std::cout << "Please input an enumerator name. (Possible values are ";
            for (Enum i = EnumInfo<Enum>::first(); i <= EnumInfo<Enum>::last(); i = static_cast<Enum>(static_cast<int>(i) + 1))
            {
                if (i != EnumInfo<Enum>::first())
                {
                    std::cout << ", ";
                }
                std::cout << EnumInfo<Enum>::names[i];
            }
            std::cout << "): ";
            std::string enumeratorName;
            std::cin >> enumeratorName;

            Enum enumValue = EnumInfo<Enum>::FromString(enumeratorName);
            std::cout << "This enumerator has the value: " << enumValue << "." << std::endl
                      << "And its name is: "         << EnumInfo<Enum>::ToString(enumValue)
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
                std::cout << "The corresponding enumerator name is: " << EnumInfo<Enum>::ToString(enumValue) << std::endl << std::flush;
            }

        }
    }
    catch (const std::exception & exc)
    {
        std::cerr << exc.what() << std::endl << std::flush;
    }

    PauseConsole("Press any key to start the next test (or quit if this was the last one).");
    std::cout << std::endl;
}


template<class TEnumType, TEnumType Enumerator>
void TestEnumerator()
{
    std::cout << "=== Enumerator Test ===" << std::endl;
    typedef typename EnumeratorInfo<TEnumType, Enumerator>::EnumType Enum;
    std::cout << "The enumerator value is: " << EnumeratorInfo<Enum, Enumerator>::value() << "." << std::endl;
    std::cout << "The enumerator name is: " << EnumeratorInfo<Enum, Enumerator>::name() << "." << std::endl;
    std::cout << "The enumerator belongs to the " << EnumInfo<Enum>::name() << " enum." << std::endl << std::endl;

    PauseConsole("Press any key to start the next test (or quit if this was the last one).");
    std::cout << std::endl;
}


int main()
{
    PauseConsole("\nWarning: This test requires user interaction. This may get on your nerves.\nPress any key to continue.");
    std::cout << std::endl;

    TestEnum<HTTPRequestMethod>();
    TestEnumerator<HTTPRequestMethod, GET>();
    TestEnumerator<HTTPRequestMethod, PUT>();

    TestEnum<Note>();
    TestEnumerator<Note, Sol>();
    TestEnumerator<Note, Si>();

    TestEnum<Color>();
    TestEnumerator<Color, Yellow>();
    TestEnumerator<Color, Orange>();

    std::cout << std::endl << "We are complete." << std::endl << std::flush;
    return 0;
}

