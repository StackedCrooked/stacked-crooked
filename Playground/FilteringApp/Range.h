#ifndef RANGE_H
#define RANGE_H


#include <cstdint>
#include <iterator>


template<typename T>
struct Range
{
    Range() : mBegin(), mEnd() {}

    Range(T* b, T* e) : mBegin(b), mEnd(e) { }

    const T& operator[](std::size_t i) const { return mBegin[i]; }
    T& operator[](std::size_t i) { return mBegin[i]; }

    T* begin() { return mBegin; }
    T* end() { return mEnd; }

    T& front() { return *begin(); }
    T& back() { return *(end() - 1); }

    uint32_t size() const { return mEnd - mBegin; }

    bool empty() const { return !size(); }

private:
    T* mBegin;
    T* mEnd;
};


template<typename FwdIt>
auto MakeRange(FwdIt b, FwdIt e) -> Range<typename std::iterator_traits<FwdIt>::value_type>
{
    return Range<typename std::iterator_traits<FwdIt>::value_type>(b, e);
}


#endif // RANGE_H
