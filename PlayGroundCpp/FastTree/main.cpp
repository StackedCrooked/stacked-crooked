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


int main()
{
    // Binary tree five levels deep
    Node<char, 2, 1> node;
    std::cout << sizeof(node) << std::endl;
    return 0;
}
