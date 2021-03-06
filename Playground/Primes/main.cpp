#include "Primes.h"
#include "Poco/Stopwatch.h"
#include "Poco/Thread.h"
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <vector>


using namespace Primes;


void SelfTest()
{
    std::vector<UInt32> primes;
    GetPrimes(100, primes);
    if (primes.size() != 25 && primes.back() != 97)
    {
        throw std::runtime_error("SelfTest failed for GetPrimes.");
    }

    std::vector<UInt32> newPrimes;
    FindPrimesInInterval(std::make_pair(100, 200), primes, newPrimes);
    if (newPrimes.size() != 21 && newPrimes.back() != 199)
    {
        throw std::runtime_error("SelfTest failed for FindPrimesInInterval.");
    }
}


void TestMultiThreaded(UInt32 inNumberOfPrimes, int inNumberOfThreads)
{
    Poco::Stopwatch stopwatch;
    stopwatch.start();
    std::vector<UInt32> primes1;
    GetPrimes(10000, primes1);
    std::vector<UInt32> primes2;
    FindPrimesInInterval_MultiThreaded(inNumberOfThreads,
                                       std::make_pair(10001, inNumberOfPrimes),
                                       primes1,
                                       primes2);
    std::cout << "Multi-threaded (" << inNumberOfThreads << " threads): " << (stopwatch.elapsed() / 1000) << "ms." << std::endl;
}


void TestSingleThreaded(UInt32 inNumberOfPrimes)
{
    std::cout << "Now calculating the first " << inNumberOfPrimes << " prime numbers single threaded." << std::endl;
    Poco::Stopwatch stopwatch;
    stopwatch.start();
    std::vector<UInt32> primes;
    GetPrimes(inNumberOfPrimes, primes);
    std::cout << "Single threaded: " << (stopwatch.elapsed() / 1000) << "ms." << std::endl;
}


int main()
{
    SelfTest();
    
    const UInt32 cNumberOfPrimes = 50 * 1000 * 1000;

    for (int i = 0; i < 16; ++i)
    {
        TestMultiThreaded(cNumberOfPrimes, i + 1);    
    }

    std::cout << "Press ENTER to quit.";
    std::cin.get();
    return 0;
}