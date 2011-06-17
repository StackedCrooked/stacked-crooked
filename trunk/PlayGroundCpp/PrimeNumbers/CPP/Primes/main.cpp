#include <iostream>
#include <vector>


typedef unsigned int UInt32;
typedef UInt32 Prime;
typedef std::vector<Prime> Primes;


UInt32 FastSqrt(UInt32 n)
{
   UInt32 root = 0, bit, trial;
   bit = (n >= 0x10000) ? 1<<30 : 1<<14;
   do
   {
      trial = root+bit;
      if (n >= trial)
      {
         n -= trial;
         root = trial+bit;
      }
      root >>= 1;
      bit >>= 2;
   } while (bit);
   return root;
}


bool IsPrime(UInt32 inNumber, const std::vector<UInt32> & inPrecedingPrimes)
{
    UInt32 squaredNumber = FastSqrt(inNumber);
    for (size_t idx = 1; idx < inPrecedingPrimes.size(); ++idx)
    {
        const Prime & primeFactor = inPrecedingPrimes[idx];

        if (primeFactor > squaredNumber)
        {
            return true;
        }

        if (inNumber % primeFactor == 0)
        {
            return false;
        }
    }
    return true;
}


void GetPrimes(size_t n, std::vector<UInt32> & ioPrimes)
{
    if (!ioPrimes.empty())
    {
        throw std::invalid_argument("ioPrimes must be empty");
    }

    if (n < 1)
    {
        return;
    }

    ioPrimes.push_back(2);
    
    for (UInt32 idx = 3; idx < n; idx += 2)
    {
        if (IsPrime(idx, ioPrimes))
        {
            ioPrimes.push_back(idx);
            if (ioPrimes.size() % 10000 == 0)
            {
                std::cout << "Found prime " << std::scientific << ioPrimes.size() << ": " << std::scientific << idx << "." << std::endl;
            }
        }
    }
}


int main()
{
    Primes primes;
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