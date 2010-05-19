#ifndef PRIMES_H_INCLUDED
#define PRIMES_H_INCLUDED

#include "Poco/Foundation.h"
#include <vector>
typedef unsigned int UInt32;

namespace Primes
{

    void GetPrimes(size_t n, std::vector<UInt32> & outPrimes);

    bool IsPrime(UInt32 inNumber, const std::vector<UInt32> & inPrecedingPrimes);

    UInt32 FastSqrt(UInt32 n);

    typedef std::pair<size_t, size_t> Interval;

    /**
     * Finds for prime number in the interval [begin, end[
     * The preceding primes argument is assumed to contain all
     * prime numbers from 2 up until at least the square root
     * of 'end'.
     */
    void FindPrimesInInterval(const Interval & inInterval,
                              const std::vector<UInt32> & inPrecedingPrimes,
                              std::vector<UInt32> & outPrimes);
    
    void FindPrimesInInterval_MultiThreaded(size_t inNumberOfThreads,
                                            const Interval & inInterval,
                                            const std::vector<UInt32> & inPrecedingPrimes,
                                            std::vector<UInt32> & outPrimes);
} // namespace Primes


#endif // PRIMES_H_INCLUDED
