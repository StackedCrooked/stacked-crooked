/**
 * Problem 006
 * -----------
 * The sum of the squares of the first ten natural numbers is:
 *   1^2 + 2^2 + ... + 10^2 = 385
 *
 * The square of the sum of the first ten natural numbers is:
 *   (1 + 2 + ... + 10)^2 = 55^2 = 3025
 *
 * Hence the difference between the sum of the squares of the first ten natural
 * numbers and the square of the sum is 3025  385 = 2640.
 *
 * Find the difference between the sum of the squares of the first one hundred
 * natural numbers and the square of the sum.
 */


/*
Solution:

Define helper functions:
  sum(n, fun)
  sum_of_squares(n)
  square_of_sum(n)


  sum_elements(generate(1, 100, square))
  boost::counting_iterator
sum_of_squares(100)
square_of_sum(100)

*/


#include <boost/iterator/counting_iterator.hpp>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <vector>
#include <stdint.h>



template<typename T>
std::ostream& operator<<(std::ostream & os, const std::vector<T> & vec)
{
    os << "[ ";
    for (const auto & v : vec)
    {
        os << v << " ";
    }
    return os << "]";
}

template<typename T>
T sq(T v) { return v * v; }

int main()
{
    std::vector<int64_t> numbers;
    std::generate_n(std::back_inserter(numbers), 100,
                    [](){ static int64_t n; return ++n; });


    auto sum_of_sq =
        std::accumulate(boost::make_counting_iterator(1),
                        boost::make_counting_iterator(100 + 1),
                        int64_t(),
                        [](int64_t & x, int64_t n) { x += n*n; return x; });

    auto sq_of_sum =
        sq(std::accumulate(boost::make_counting_iterator(1),
                        boost::make_counting_iterator(100 + 1),
                        int64_t(),
                        [](int64_t & x, int64_t n) { x += n; return x; }));

    std::cout << "sum_of_sq: " << sum_of_sq << std::endl;
    std::cout << "sq_of_sum: " << sq_of_sum << std::endl;
    std::cout << "(sum_of_sq - sq_of_sum): " << (sum_of_sq - sq_of_sum) << std::endl;
}
