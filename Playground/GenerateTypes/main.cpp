#define TYPEDEF(Modifiertypedef unsigned short ushort;

#define GENERATE_TYPE(types, modifier_sets) \
    BOOST_PP_
    
GENREATE_TYPES((char)(short)(int)(long),


GENERATE_TYPES(
        (char)(short)(int)(long), // types
        (
            (unsigned)() // unsigned or not
        )
        (
            (const)() // const or not
        )
        (
            (volatile)() // volatile or not
        )
)
