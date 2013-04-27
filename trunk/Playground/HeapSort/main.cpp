#include <iostream>
#include <vector>


template<typename T>
using Heap = std::vector<T>;


template<typename T>
void BuildMaxHeap(Heap<T> & heap)
{
    heap.back();
}


// MaxHeapify:
// BuildMaxHeap: applies MaxHeapify on the heap nodes, starting with the leaf nodes.


int main()
{
}
