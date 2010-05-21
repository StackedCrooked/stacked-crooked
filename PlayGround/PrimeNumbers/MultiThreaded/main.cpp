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


void TestMultiThreaded(UInt32 inNumberOfPrimes)
{
    Poco::Stopwatch stopwatch;
    stopwatch.start();
    std::vector<UInt32> primes1;
    GetPrimes(10000, primes1);
    const size_t cNumberOfThreads = 4;
    std::vector<UInt32> primes2;
    FindPrimesInInterval_MultiThreaded(cNumberOfThreads,
                                       std::make_pair(10001, inNumberOfPrimes),
                                       primes1,
                                       primes2);
    std::cout << "Multi-threaded (3 threads): " << (stopwatch.elapsed() / 1000) << "ms." << std::endl;
}


void TestSingleThreaded(UInt32 inNumberOfPrimes)
{
    Poco::Stopwatch stopwatch;
    stopwatch.start();
    std::vector<UInt32> primes;
    GetPrimes(inNumberOfPrimes, primes);
    std::cout << "Single threaded: " << (stopwatch.elapsed() / 1000) << "ms." << std::endl;
}


int main()
{
    SelfTest();

    const UInt32 cNumberOfPrimes = 10 * 1000 * 1000;
    TestSingleThreaded(cNumberOfPrimes);    
    TestMultiThreaded(cNumberOfPrimes);

    std::cout << "Ok. Press any character + ENTER to quit." << std::endl;
    char c;
    std::cin >> c;
    return 0;
}