/**
 * Problem 004
 * ===========
 * A palindromic number reads the same both ways. The largest palindrome made
 * from the product of two 2-digit numbers is 9009 = 91 x 99.
 *
 * Find the largest palindrome made from the product of two 3-digit numbers.
 */


#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdint.h>

bool is_palindrome(const std::string & str)
{
    std::string copy(str);
    std::reverse(copy.begin(), copy.end());
    return str == copy;
}

template<typename T>
bool is_number_palindrome(T value)
{
    return is_palindrome(std::to_string(value));
}


int main()
{
    int max_palindrome = 0;
    for (int i = 0; i < 1000; ++i)
    {
        for (int j = 0; j <= i; ++j)
        {
            auto a = 999 - i;
            auto b = 999 - j;
            auto prod = a * b;
            if (is_number_palindrome(prod))
            {
                //std::cout << a << " x " << b << " = " << prod << std::endl;
                max_palindrome = std::max(max_palindrome, prod);
            }
        }
    }
    std::cout << "max_palindrome: " << max_palindrome << std::endl;
}
