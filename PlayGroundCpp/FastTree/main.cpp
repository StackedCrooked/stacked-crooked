#include <boost/bind.hpp>
#include <algorithm>
#include <functional>
#include <cassert>
#include <iostream>


template<typename T, unsigned N, unsigned MaxDepth, unsigned Depth = 0>
struct Node
{
    T data;
    typedef Node<T, N, MaxDepth, Depth + 1> Child;
    Child children[N];
};

template<typename T, unsigned N, unsigned MaxDepth>
struct Node<T, N, MaxDepth, MaxDepth>
{
    T data;
};


template<unsigned N>
struct Buffer
{
    char data[N];
};


struct Data
{
    int mScore;
    Buffer<200> mGrid;
};


template<typename Value, typename Cmp>
std::size_t Insert(Value * buffer, std::size_t size, std::size_t maxsize, Value & value, Cmp cmp)
{
    std::cout << "size: " << size << ", maxsize: " << maxsize << std::endl;
    if (maxsize == 0)
    {
        return size;
    }

    if (size == 0)
    {
        buffer[0] = value;
        return 1;
    }
    
    Value & first = buffer[0];
    if (cmp(value, first))
    {
        std::swap(value, first);
        return 1 + Insert(buffer + 1, size - 1, maxsize - 1, value, cmp);
    }
    else
    {
        return 1 + Insert(buffer + 1, size - 1, maxsize - 1, value, cmp);
    }
}


template<typename Value>
std::size_t Insert(Value * buffer, std::size_t size, std::size_t maxsize, Value & value)
{
    Insert(buffer, size, maxsize, value, std::less<int>());
}


template<typename T>
bool IsGreaterThanOrEqualTo(const T & a, const T & b)
{
    return a >= b;
}

int main()
{
    const std::size_t cBufferSize = 5;
    int buffer[cBufferSize];
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 0;
    buffer[4] = 0;


    int values[] = {11, 24, 7, 31, 41, 93, 91, 62, 19, 91, 98, 6, 63, 9, 23, 42, 46, 18, 89, 18 };
    int * value = values;

    std::size_t size = 0;
    for (int idx = 0; idx < sizeof(values)/sizeof(int); ++idx)
    {
        size = Insert(&buffer[0], size, cBufferSize, *value++, boost::bind(&IsGreaterThanOrEqualTo<int>, _1, _2));
        assert(size <= cBufferSize);
        std::cout << "Size is now " << size << ": ";
        for (std::size_t j = 0; j < size; ++j)
        {
            if (j != 0)
            {
                std::cout << ", ";
            }
            std::cout << int(buffer[j]);
        }
        std::cout << std::endl;
    }
    return 0;
}
