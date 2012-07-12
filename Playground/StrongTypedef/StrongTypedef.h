#ifndef FUTILE_STRONG_TYPEDEF_H
#define FUTILE_STRONG_TYPEDEF_H


namespace Futile {


/**
 * Helper class for the FUTILE_STRONG_TYPEDEF macro.
 */
template<typename Differentiator, typename T>
struct StrongTypedef
{
    explicit StrongTypedef(const T & inValue = T()) : mValue(inValue) { }

    typedef const T& CRef;
    typedef T& Ref;

    operator CRef() const { return mValue; }
    operator Ref ()       { return mValue; }

    //typedef StrongTypedef<Differentiator, T> This;

    //friend bool operator==(const This & lhs, const This & rhs) { return lhs.mValue == rhs.mValue; }
    //friend bool operator< (const This & lhs, const This & rhs) { return lhs.mValue <  rhs.mValue; }

    T mValue;
};


} // namespace Futile


/**
 * Create a "strong" typedef.
 *
 * The resulting type is a unique type and
 * can be used for differentating overloads.
 *
 * @example FUTILE_STRONG_TYPEDEF(int, Width)
 */
#define FUTILE_STRONG_TYPEDEF(TYPE, NAME) \
    typedef Futile::StrongTypedef<struct Futile##_##NAME##_##TYPE, TYPE> NAME;


#endif // FUTILE_STRONG_TYPEDEF_H
