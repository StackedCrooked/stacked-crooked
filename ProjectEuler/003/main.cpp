#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdint.h>

/**
Q: The prime factors of 13195 are 5, 7, 13 and 29.
   What is the largest prime factor of the number 600851475143 ?

- Largest = largest prime factor smaller than sqrt(600851475143)
Largest theoretical: .

Example:
12 6 3 3
11 11
10 5 1
9  3 3 1
8 4 2 1

Algorithms:
primes = find_primes from 0 to sqrt(X)
for each prime
    while X divisible by n then
        X /= n
        result = max(result, n)
    end if
    if X < 2 then return n
end for


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


void next_prime(std::vector<uint64_t> & preceding)
{
    if (preceding.empty())
    {
        preceding.push_back(2);
        return;
    }

    if (preceding.back() == 2)
    {
        preceding.push_back(3);
        return;
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
            return;
        }
    }
}


//std::vector<uint64_t> find_primes(uint64_t max)
//{
//    std::vector<uint64_t> result;
//    while (next_prime(result) < max);
//    return result;
//}


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


uint64_t find_largest_prime_factor_of(uint64_t x)
{
    std::vector<uint64_t> primes = {2};
    for (;;)
    {
        auto prime = primes.back();
        if (prime >= x)
        {
            return prime;
        }

        while (x % prime == 0 && x != 0)
        {
            x /= prime;
        }

        if (x < 2)
        {
            return prime;
        }

        next_prime(primes);
    }

    assert(false);
}


int main()
{
    assert(find_largest_prime_factor_of(2) == 2);
    assert(find_largest_prime_factor_of(3) == 3);
    assert(find_largest_prime_factor_of(4) == 2);
    assert(find_largest_prime_factor_of(5) == 5);
    assert(find_largest_prime_factor_of(6) == 3);
    assert(find_largest_prime_factor_of(7) == 7);
    assert(find_largest_prime_factor_of(8) == 2);
    assert(find_largest_prime_factor_of(9) == 3);
    assert(find_largest_prime_factor_of(10) == 5);
    assert(find_largest_prime_factor_of(11) == 11);
    std::cout << "Self-test complete" << std::endl;


    std::cout << "The largest prime factor of " << 600851475143 << " is " << find_largest_prime_factor_of(600851475143) << "." << std::endl;
}
