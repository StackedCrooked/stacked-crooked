#include <string>
#include <vector>
#include <windows.h>
#include <assert.h>


int * binary_search(int * first, int * last, int x)
{
    if (x < *first)
    {
        return 0;
    }
    else if (x > *last)
    {
        return 0;
    }
    else
    {
        int * middle = first + (last-first)/2;
        if (x < *middle)
        {
            return binary_search(first, middle - 1, x);
        }
        else if (x > *middle)
        {
            return binary_search(middle + 1, last, x);
        }
        else
        {
            return middle;
        }
    }
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    std::vector<int> numbers;
    numbers.push_back(1);
    numbers.push_back(2);
    numbers.push_back(4);
    numbers.push_back(8);
    numbers.push_back(16);

    // found
    assert(1 == *binary_search(&numbers[0], &numbers[numbers.size()-1], 1));
    assert(2 == *binary_search(&numbers[0], &numbers[numbers.size()-1], 2));
    assert(4 == *binary_search(&numbers[0], &numbers[numbers.size()-1], 4));
    assert(8 == *binary_search(&numbers[0], &numbers[numbers.size()-1], 8));
    assert(16 == *binary_search(&numbers[0], &numbers[numbers.size()-1], 16));

    // not found should return 0
    assert(!binary_search(&numbers[0], &numbers[numbers.size()-1], 0));
    assert(!binary_search(&numbers[0], &numbers[numbers.size()-1], 3));
    assert(!binary_search(&numbers[0], &numbers[numbers.size()-1], 7));
    assert(!binary_search(&numbers[0], &numbers[numbers.size()-1], 11));
    assert(!binary_search(&numbers[0], &numbers[numbers.size()-1], 17));

    return 0;
}
