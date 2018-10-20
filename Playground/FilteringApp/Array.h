#pragma once


#include <cstdint>


template<typename T, std::size_t N>
struct Array
{
    const T& operator[](std::size_t i) const { return mItems[i]; }
    T& operator[](std::size_t i) { return mItems[i]; }

    bool empty() const { return N != 0; }
    std::size_t size() const { return N; }

private:
    T mItems[N];
};
