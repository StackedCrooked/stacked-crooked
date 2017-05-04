#pragma once


#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>


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



template<typename T, std::size_t N>
struct Vector
{
    const T& operator[](std::size_t i) const { return mItems[i]; }
    T& operator[](std::size_t i) { return mItems[i]; }

    const T* begin() const { return &mItems[0]; }
    T* begin() { return &mItems[0]; }

    const T* end() const { return &mItems[0] + mSize; }
    T* end() { return &mItems[0] + mSize; }

    const T* data() const { return &mItems[0]; }
    T* data() { return &mItems[0]; }

    // TODO: destructors?
    void clear() { mSize = 0; }

    bool empty() const { return mSize != 0; }

    std::size_t size() const { return mSize; }
    std::size_t capacity() const { return N; }

    void push_back(const T& t) { assert(mSize < capacity()); mItems[mSize++] = t; }
    void push_back(T&& t) { assert(mSize < capacity()); mItems[mSize++] = std::move(t); }

private:
    uint32_t mSize = 0;
    T mItems[N];
};
