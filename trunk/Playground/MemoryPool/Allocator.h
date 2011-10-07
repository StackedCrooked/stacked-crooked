#ifndef allocator_H
#define allocator_H


#include "Pool.h"
#include <memory>
#include <numeric>



namespace nonstd {


template<typename T>
struct allocator
{
    typedef std::allocator<T> std_allocator;
    typedef typename std_allocator::value_type      value_type;
    typedef typename std_allocator::size_type       size_type;
    typedef typename std_allocator::difference_type difference_type;
    typedef typename std_allocator::reference       reference;
    typedef typename std_allocator::const_reference const_reference;
    typedef typename std_allocator::pointer         pointer;
    typedef typename std_allocator::const_pointer   const_pointer;

    allocator() throw() {}

    allocator(const allocator &) throw() {}

    template<class U>
    allocator(const allocator<U> & ) throw() {}

    ~allocator() throw() {}

    template<typename U>
    struct rebind
    {
        typedef allocator<U> other;
    };

    pointer       address(reference value)       const { return &value; }
    const_pointer address(const_reference value) const { return &value; }

    inline pointer allocate(size_type n, typename std::allocator<void>::const_pointer = 0)
    {
        return Pool::Get().allocate<T>(n);
    }

    inline void deallocate(pointer p, size_type n)
    {
        Pool::Get().deallocate(p, n);
    }

    inline size_type max_size() const
    {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    inline void construct(pointer p, const T & t)
    {
        new(p) T (t);
    }

    inline void destroy(pointer p)
    {
        p->~T();
    }
};


template<typename T>
inline bool operator==(const allocator<T> &, const allocator<T> & )
{
    return true; // types are equal
}

template<typename T>
inline bool operator!=(const allocator<T> & lhs, const allocator<T> & rhs)
{
    return !operator==(lhs, rhs);
}

template<typename T, typename U>
inline bool operator==(const allocator<T> &, const allocator<U> & )
{
    return false; // types are not equal
}

template<typename T, typename U>
inline bool operator!=(const allocator<T> & lhs, const allocator<U> & rhs)
{
    return !operator ==(lhs, rhs);
}


typedef std::basic_string< char, std::char_traits<char>, nonstd::allocator<char> > string;


} // namespace nonstd


#endif // allocator_H
