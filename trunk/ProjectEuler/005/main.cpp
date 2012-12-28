/**
 * Problem 005
 * ===========
 * 2520 is the smallest number that can be divided by each of the numbers from 1 to 10 without any remainder. 
 * What is the smallest positive number that is evenly divisible by all of the numbers from 1 to 20?
 */


#include <cmath>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>
#include <stdint.h>


/*
 * Algorithm:
 * For n in 2 .. 20
 *   find prime factors of n
 *   each factor has a count:
 *     e.g 20: 2 * 2 * 5 => 2 has a count of 2, 5 has a count of 1
 *   store the prime factors in a map using factor count as key
 *      20: map[2] = std::max(map[2], 2);
 *          map[5] = std::max(map[5], 1);
 *      general: map[prime_factor] = std::max(map[prime_factor], count(prime_factor))
 *
 * The product of all values of the map is the smallest positive number that is evenly divisible by all numbers from 1 to 20.
 */



bool is_prime(uint64_t n, const std::vector<uint64_t> & preceding)
{
    // precondition: "preceding" contains all primes < n

    for (const auto & p : preceding)
    {
        if (n % p == 0)
        {
            return false;
        }
    }
    return true;
}


uint64_t next_prime(std::vector<uint64_t> & preceding)
{
    if (preceding.empty())
    {
        preceding.push_back(2);
        return preceding.back();
    }

    if (preceding.back() == 2)
    {
        preceding.push_back(3);
        return preceding.back();
    }

    uint64_t n = preceding.back();

    // check range
    if (n >= uint64_t(-2))
    {
        throw std::out_of_range("n: " + std::to_string(n));
    }

    // algorithm:
    //   loop:
    //      add 2 to n
    //      if n is a prime then return n
    //      repeat
    for(;;)
    {
        n += 2;
        if (is_prime(n, preceding))
        {
            preceding.push_back(n);
            return preceding.back();
        }
    }
}


template<typename T, typename U>
std::ostream& operator<<(std::ostream& os, const std::map<T, U> & m)
{
    os << "(";
    for (const auto & p : m)
    {
        os << "(" << p.first << " " << p.second << ")";
    }
    return os << ")";
}


typedef unsigned count_type;
typedef unsigned value_type;


std::map<value_type, count_type> get_prime_factors(unsigned n)
{
    std::map<value_type, count_type> result;
    std::vector<uint64_t> pre = {2};
    for (unsigned i = 2; i <= n; i = next_prime(pre))
    {
        auto copy = n;
        while (copy % i == 0)
        {
            result[i]++;
            copy /= i;
        }
    }
    std::cout << "prime factors for " << n << ": " << result << std::endl;
    return result;
}

unsigned get_smallest_evenly_divisible(unsigned n)
{
    std::map<value_type, count_type> result;
    for (unsigned i = 2; i <= n; ++i)
    {
        for (const auto & value_and_count : get_prime_factors(i))
        {
            auto value = value_and_count.first;
            auto count = value_and_count.second;
            result[value] = std::max(result[value], count);
        }
    }

    unsigned product = 1;
    for (const auto & p : result)
    {
        for (auto i = 0u; i < p.second; ++i)
        {
            product *= p.first;
        }
    }
    return product;
}

 int main()
 {
     std::cout << "get_smallest_evenly_divisible(20) = " << get_smallest_evenly_divisible(20) << std::endl;
 }
