#include <boost/assert.hpp>
#include <boost/unordered_map.hpp>
#include <boost/preprocessor.hpp>
#include <iostream>
#include <string>

//
// PP_NARG returns the number of arguments.
// Supports up until 64 arguments.
//
#define PP_NARG(...) \
    PP_NARG_(__VA_ARGS__,PP_RSEQ_N())

#define PP_NARG_(...) \
    PP_ARG_N(__VA_ARGS__)

#define PP_ARG_N( \
    _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
    _61,_62,_63,N,...) N

#define PP_RSEQ_N() \
    63,62,61,60,                   \
    59,58,57,56,55,54,53,52,51,50, \
    49,48,47,46,45,44,43,42,41,40, \
    39,38,37,36,35,34,33,32,31,30, \
    29,28,27,26,25,24,23,22,21,20, \
    19,18,17,16,15,14,13,12,11,10, \
    9,8,7,6,5,4,3,2,1,0


#define ENUM_IMPL_MEMBER(R, Aux, I, El) \
    BOOST_PP_COMMA_IF(I) El

#define ENUM_IMPL_STRING(R, Aux, I, El) \
    BOOST_PP_COMMA_IF(I) BOOST_PP_STRINGIZE(El)

#define ENUM_IMPL_REVMAP(R, Aux, I, El) \
    cv[BOOST_PP_STRINGIZE(El)] = El;

#define ENUM_IMPL(Name, Memb) \
    enum Name { BOOST_PP_LIST_FOR_EACH_I(ENUM_IMPL_MEMBER, 0, Memb) }; \
    Name Name##Array[] = { BOOST_PP_LIST_FOR_EACH_I(ENUM_IMPL_MEMBER, 0, Memb) }; \
    std::size_t Name##Count = sizeof(Name##Array) / sizeof(Name); \
    inline std::string EnumToString(Name x) { \
        static std::string cv[] = { BOOST_PP_LIST_FOR_EACH_I(ENUM_IMPL_STRING, 0, Memb) }; \
        BOOST_ASSERT(x >= 0 && x < BOOST_PP_LIST_SIZE(Memb)); \
        return cv[x]; \
    } \
    template <> inline Name StringToEnum<Name>(const std::string& x) { \
        static boost::unordered_map<std::string, Name> cv; \
        static bool ready = false; \
        if (!ready) { \
            BOOST_PP_LIST_FOR_EACH_I(ENUM_IMPL_REVMAP, 0, Memb) \
            ready = true; \
        } \
        BOOST_ASSERT(cv.find(x) != cv.end()); \
        return cv[x]; \
    }

template <typename T> T StringToEnum(const std::string &);

#define ENUM(Name, ...) \
    ENUM_IMPL(Name, BOOST_PP_TUPLE_TO_LIST(PP_NARG(__VA_ARGS__), (__VA_ARGS__)))


//
// Define the "Color" enum
//
ENUM(Color, Red, Orange, Yellow, Green, Blue, Indigo, Violet)


//
// Define the "Note" enum
//
ENUM(Note, Do, Re, Mi, Fa, Sol, La, Si)


int main()
{
    for (std::size_t idx = 0; idx < ColorCount; ++idx)
    {
        if (idx != 0)
        {
            std::cout << ", ";
        }
        Color color = ColorArray[idx];
        std::cout << EnumToString(color) << ": " << color;
    }
    std::cout << std::endl << "Color count: " << ColorCount << std::endl << std::endl;

    for (std::size_t idx = 0; idx < NoteCount; ++idx)
    {
        if (idx != 0)
        {
            std::cout << ", ";
        }
        Note note = NoteArray[idx];
        std::cout << EnumToString(note) << ": " << note;
    }
    std::cout << std::endl << "Note count: " << ColorCount << std::endl;
    return 0;
}

