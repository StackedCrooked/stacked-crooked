#include <iostream>
#include <vector>


typedef unsigned int           UInt32;
typedef unsigned __int64       UInt64;
typedef UInt64 Prime;
typedef std::vector<Prime> Primes;

void GetPrimes(UInt32 n, std::vector<UInt64> & ioPrimes)
{
    if (!ioPrimes.empty())
    {
        throw std::invalid_argument("ioPrimes must be empty");
    }

    if (n == 0)
    {
        return;
    }

    ioPrimes.push_back(2);
    
    UInt64 n2 = n*n;
    for (UInt64 idx = 3; idx < n; idx += 2)
    {
        bool currentIdxIsPrime = true;
        for (UInt32 primeIdx = 1; primeIdx < ioPrimes.size(); ++primeIdx)
        {
            const Prime & currentPrime = ioPrimes[primeIdx];
            if (currentPrime > n2)
            {
                break;
            }

            if (idx % currentPrime == 0)
            {
                currentIdxIsPrime = false;
                break;
            }
        }
        if (currentIdxIsPrime)
        {
            ioPrimes.push_back(idx);
            if (ioPrimes.size() % 1000 == 0)
            {
                std::cout << "Found prime " << ioPrimes.size() << ": " << idx << "." << std::endl;
            }
        }
    }
}

int main()
{
    Primes primes;
    GetPrimes(1000 * 1000, primes);

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