#include "Primes.h"
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


void CompareSpeeds(UInt32 inNumberOfPrimes)
{
    std::cout << "Calculating the first " << inNumberOfPrimes << " number of primes." << std::endl;
    {
        std::time_t startTime = time(0);
        std::vector<UInt32> primes;
        GetPrimes(inNumberOfPrimes, primes);
        std::time_t elapsed = time(0) - startTime;
        std::cout << "Test 1 took " << elapsed << "s." << std::endl;

    }

    {
        std::time_t startTime = time(0);

        std::vector<UInt32> primes1;
        GetPrimes(10000, primes1);


        const size_t cNumberOfThreads = 3;
        std::vector<UInt32> primes2;
        FindPrimesInInterval_MultiThreaded(cNumberOfThreads,
                                           std::make_pair(10001, inNumberOfPrimes),
                                           primes1,
                                           primes2);
        std::time_t elapsed = time(0) - startTime;
        std::cout << "Test 2 took " << elapsed << "s." << std::endl;
    }
}


int main()
{
    SelfTest();

    CompareSpeeds(10 * 1000 * 1000);

    std::cout << "Ok. Press any character + ENTER to quit." << std::endl;
    char c;
    std::cin >> c;
    return 0;
}