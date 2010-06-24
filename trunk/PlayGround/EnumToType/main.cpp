
enum Type
{
    Type_Int,
    Type_Bool,
    Type_String
};

template<Type type>
struct EnumToType
{
};

template <>
struct EnumToType<Type_Int>
{
    typedef int Type;
};

template <>
struct EnumToType<Type_Bool>
{
    typedef bool Type;
};

template<class T>
struct TypeToEnum
{
};

template<>
struct TypeToEnum<int>
{
    static const Type value = Type_Int;
};

template<>
struct TypeToEnum<bool>
{
    static const Type value = Type_Bool;
};

int main(int argc, char** argv)
{
    EnumToType<Type_Int>::Type a = 3;
    EnumToType<Type_Bool>::Type b = false;


    Type intType = TypeToEnum<int>::value;
    Type boolType = TypeToEnum<bool>::value;
    
    return 0;
}
