#include "Primes.h"
#include "Poco/Thread.h"
#include <iostream>
#include <vector>


using namespace Primes;


int main()
{
    std::vector<UInt32> primes;
    GetPrimes(100 * 1000 * 1000, primes);

    std::cout << "Found " << primes.size() << " primes." << std::endl;
    if (!primes.empty())
    {
        std::cout << "Last one was nr " << primes.size() << ": " << primes.back() << std::endl;
    }

    std::cout << "Input any character followed with ENTER to stop.";
    char c;
    std::cin >> c;
    return 0;
}