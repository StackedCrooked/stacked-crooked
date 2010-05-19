#ifndef PRIMES_H_INCLUDED
#define PRIMES_H_INCLUDED

#include <vector>
typedef unsigned int UInt32;

namespace Primes
{

    void GetPrimes(size_t n, std::vector<UInt32> & ioPrimes);

    bool IsPrime(UInt32 inNumber, const std::vector<UInt32> & inPrecedingPrimes);

    UInt32 FastSqrt(UInt32 n);

} // namespace Primes


#endif // PRIMES_H_INCLUDED
