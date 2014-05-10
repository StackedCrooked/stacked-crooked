/**
 * Problem 14
 * ----------
 * The following iterative sequence is defined for the set of positive integers:
 *   n  n/2 (n is even)
 *   n  3n + 1 (n is odd)
 *
 * Using the rule above and starting with 13, we generate the following sequence:
 *   13  40  20  10  5  16  8  4  2  1
 *
 * It can be seen that this sequence (starting at 13 and finishing at 1)
 * contains 10 terms. Although it has not been proved yet (Collatz Problem), it
 * is thought that all starting numbers finish at 1.
 *
 * Which starting number, under one million, produces the longest chain?
 */


#include <iostream>


constexpr unsigned get_collatz_sequence_length_impl(unsigned n)
{
    return n == 1 ? 1
                  : 1 + get_collatz_sequence_length_impl(n % 2 == 0 ? n/2 : 3*n + 1);
}

constexpr unsigned get_collatz_sequence_length(unsigned n)
{
    return n == 0 ? 0 : get_collatz_sequence_length_impl(n);
}

int main()
{
    unsigned max = 0;
    for (unsigned i = 1; i != 1000000; ++i)
    {
        auto len = get_collatz_sequence_length(i);
        if (len > max)
        {
            max = len;
            std::cout << i << ": " << max << std::endl;
        }
    }
}
