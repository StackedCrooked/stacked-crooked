#ifndef STRONG_TYPEDEF_H
#define STRONG_TYPEDEF_H


#include <boost/operators.hpp>


#define STRONG_TYPEDEF(Type, Name) \
    struct Name : boost::operators<Name> { \
        Name() : data() {} \
        template<typename ...Args> \
        explicit Name(Args && ...args) : data(std::forward<Args>(args)...) {} \
        operator const Type&() const { return data; } \
        operator Type&() { return data; } \
        bool operator<(const Name & rhs) { return data < rhs.data; } \
        bool operator==(const Name & rhs) { return data == rhs.data; } \
        Type data; \
        friend std::ostream & operator<<(std::ostream & os, const Name & obj) { return os << obj.data; } \
    };


#endif // STRONG_TYPEDEF_H
