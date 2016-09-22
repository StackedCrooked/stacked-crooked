#include <cassert>
#include <iostream>
#include <string>
#include <vector>


int* binary_search_impl(int* b, int* e, int x, int* end_of_input_range)
{
    if (b == e)
    {
        return end_of_input_range;
    }
    if (x < *b)
    {
        return end_of_input_range;
    }
    
    if (x == *b)
    {
        return b;
    }

    auto len = e - b;

    int* m = b + len / 2;

    if (x < *m)
    {
        return binary_search_impl(b, m, x, end_of_input_range);
    }
    else if (x > *m)
    {
        return binary_search_impl(m + 1, e, x, end_of_input_range);
    }
    else
    {
        return m;
    }
}

int* binary_search(int* b, int* e, int x)
{
    return binary_search_impl(b, e, x, e);
}

#define ASSERT_EQ(x, y) check_eq(__FILE__, __LINE__, x, y)
#define ASSERT_NE(x, y) check_ne(__FILE__, __LINE__, x, y)

template<typename T, typename U>
void check_eq(const char* file, int line, T&& x, U&& y)
{
    if (x == y)
    {
        std::cout << "PASS: " << x << " == " << y << std::endl;
    }
    else
    {
        std::cout << file << ":" << line << ": FAIL: " << x << " != " << y << std::endl;
    }
}

template<typename T, typename U>
void check_ne(const char* file, int line, T&& x, U&& y)
{
    if (x != y)
    {
        std::cout << "PASS: " << x << " != " << y << std::endl;
    }
    else
    {
        std::cout << file << ":" << line << ": FAIL: " << x << " == " << y << std::endl;
    }
}

int main()
{
    std::vector<int> numbers;
    numbers.push_back(1);
    numbers.push_back(2);
    numbers.push_back(4);
    numbers.push_back(8);
    numbers.push_back(16);

    auto b = numbers.data();
    auto e = b + numbers.size();

    // found
    ASSERT_NE(binary_search(b, e, 1), e);
    ASSERT_NE(binary_search(b, e, 2), e);
    ASSERT_NE(binary_search(b, e, 4), e);
    ASSERT_NE(binary_search(b, e, 8), e);
    ASSERT_NE(binary_search(b, e, 16), e);

    ASSERT_EQ(*binary_search(b, e, 1), 1);
    ASSERT_EQ(*binary_search(b, e, 2), 2);
    ASSERT_EQ(*binary_search(b, e, 4), 4);
    ASSERT_EQ(*binary_search(b, e, 8), 8);
    ASSERT_EQ(*binary_search(b, e, 16), 16);

    // not found should return 0
    ASSERT_EQ(binary_search(b, e, 0), e);
    ASSERT_EQ(binary_search(b, e, 3), e);
    ASSERT_EQ(binary_search(b, e, 7), e);
    ASSERT_EQ(binary_search(b, e, 11), e);
    ASSERT_EQ(binary_search(b, e, 17), e);

    std::cout << "All tests succeeded." << std::endl;

    return 0;
}
