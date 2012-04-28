#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>
 
 
template<typename T>
struct Buffer
{
    typedef std::size_t size_type;
    typedef T value_type;
 
    Buffer() :
        _size(),
        _data()
    {
    }
 
    Buffer(size_type len, const T & value = T()) :
        _size(len),
        _data(static_cast<T*>(operator new(_size * sizeof(T))))
    {
        std::uninitialized_fill(begin(), end(), value); 
    }
 
    Buffer(const Buffer & rhs) :
        _size(rhs._size),
        _data(static_cast<T*>(operator new(_size * sizeof(T))))
    {
        std::copy(rhs.begin(), rhs.end(), begin());
    }
 
    ~Buffer()
    {
        delete [] _data;
    }
 
    Buffer & operator=(Buffer rhs) {
        swap(rhs);
        return *this;
    }
 
    void swap(Buffer & rhs) {
        using std::swap;
        swap(_size, rhs._size);
        swap(_data, rhs. _data);
    }
 
    size_type size() const { return _size; }
 
    const T * data() const { return _data; }
 
    T & operator[](size_type idx) { return _data[idx]; }
 
    const T & operator[](size_type idx) const { return _data[idx]; }
 
    struct iterator : std::iterator<std::forward_iterator_tag, value_type>
    {
        value_type * _data;

        iterator(value_type * data) : _data(data) {}

        value_type & operator*() { return *_data; }

        const value_type & operator*() const { return *_data; }

        iterator & operator++() { ++_data; return *this; }
 
        friend bool operator==(const iterator & lhs, const iterator & rhs) { return lhs._data == rhs._data; }

        friend bool operator!=(const iterator & lhs, const iterator & rhs) { return !(lhs == rhs); }
    };
 
    iterator begin() { return iterator(_data); }
 
    iterator end() { return iterator(_data + size()); }
 
    struct const_iterator : std::iterator<std::forward_iterator_tag, value_type>
    {
        const value_type * _data;
        const_iterator(const value_type * data) : _data(data) {}
        const value_type & operator*() const { return *_data; }
        const_iterator & operator++() { ++_data; return *this; }
 
        friend bool operator==(const const_iterator & lhs, const const_iterator & rhs) { return lhs._data == rhs._data; }

        friend bool operator!=(const const_iterator & lhs, const const_iterator & rhs) { return !(lhs == rhs); }
    };
 
    const_iterator begin() const { return const_iterator(_data); }
 
    const_iterator end() const { return const_iterator(_data + size()); }
 
    size_type _size;
    T * _data;
};
 
 
struct Test { Test(int) {} };
 

int main()
{
    Buffer<Test> emptyTest;
    
    Buffer<Test> b1(5, Test(0));
    Buffer<Test> b2(b1);
    Buffer<Test> b3(b2);
    b1 = b2 = b3;
    
 
    for (std::size_t idx = 0; idx < 2; ++idx)
    {
        std::cout << "Uninitialized: " << std::endl;
        Buffer<int>  raw(32);
        for (Buffer<int>::size_type idx = 0; idx != raw.size(); ++idx)
        {
            if (idx != 0) {
                std::cout << " ";
            }
            if (idx != 0 && idx % 8 == 0) {
                std::cout << std::endl;
            }
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(raw[idx]);
            raw[idx] = idx;
        }
        std::cout << std::endl << std::endl;
    }
 
    for (std::size_t idx = 0; idx < 2; ++idx)
    {
        std::cout << "Initialized to 1: " << std::endl;
        Buffer<int>  raw(32, 1);
        for (Buffer<int>::size_type idx = 0; idx != raw.size(); ++idx)
        {
            if (idx != 0) {
                std::cout << " ";
            }
            if (idx != 0 && idx % 8 == 0) {
                std::cout << std::endl;
            }
            std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(raw[idx]);
            raw[idx] = idx;
        }
        std::cout << std::endl << std::endl;
    }
}

