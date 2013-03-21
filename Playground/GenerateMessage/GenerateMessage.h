#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/cat.hpp>

#define SEQ_FIRST(Seq) BOOST_PP_SEQ_ELEM(0, Seq)
#define SEQ_SECOND(Seq) BOOST_PP_SEQ_ELEM(1, Seq)

#define GENERATE_COMMA() ,
#define GENERATE_SEMICOLON() ;
#define GENERATE_NOTHING()

#define FIRST_(r, GenSep, Pair) GenSep() SEQ_FIRST(Pair)
#define SECOND_(r, GenSep, Pair) GenSep() SEQ_SECOND(Pair)

#define MAP_FIRST(Fields, GenSep) \
    SEQ_FIRST(BOOST_PP_SEQ_HEAD(Fields)) BOOST_PP_SEQ_FOR_EACH(FIRST_, GenSep, BOOST_PP_SEQ_TAIL(Fields))

#define MAP_SECOND(Fields, GenSep) \
    SEQ_SECOND(BOOST_PP_SEQ_HEAD(Fields)) BOOST_PP_SEQ_FOR_EACH(SECOND_, GenSep, BOOST_PP_SEQ_TAIL(Fields))

#define GEN_CONSTRUCTOR_ARGUMENT(r, GenSep, Field) \
    GenSep() const SEQ_FIRST(Field) & SEQ_SECOND(Field)

#define GEN_CONSTRUCTOR_ARGUMENTS(Fields) \
    const SEQ_FIRST(BOOST_PP_SEQ_HEAD(Fields)) & SEQ_SECOND(BOOST_PP_SEQ_HEAD(Fields)) BOOST_PP_SEQ_FOR_EACH(GEN_CONSTRUCTOR_ARGUMENT, GENERATE_COMMA, BOOST_PP_SEQ_TAIL(Fields))

#define GEN_METHOD_(r, GenSep, Index, Field) \
    const SEQ_FIRST(Field) & BOOST_PP_CAT(get_, SEQ_SECOND(Field)) () const { return std::get< Index >(*this); }

#define GEN_METHODS(Fields) \
    BOOST_PP_SEQ_FOR_EACH_I(GEN_METHOD_, GENERATE_SEMICOLON, Fields)

#define GENERATE_MESSAGE(Name, Fields) \
    struct Name : std::tuple< MAP_FIRST(Fields, GENERATE_COMMA) > { \
        Name(GEN_CONSTRUCTOR_ARGUMENTS(Fields)) : \
                std::tuple< MAP_FIRST(Fields, GENERATE_COMMA) >(MAP_SECOND(Fields, GENERATE_COMMA)) {} \
        GEN_METHODS(Fields) \
    }
