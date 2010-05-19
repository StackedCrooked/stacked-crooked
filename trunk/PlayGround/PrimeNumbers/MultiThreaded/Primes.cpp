#include "Primes.h"


namespace Primes
{

    bool IsPrime(UInt32 inNumber, const std::vector<UInt32> & inPrecedingPrimes)
    {
        UInt32 squaredNumber = FastSqrt(inNumber);
        for (size_t idx = 1; idx < inPrecedingPrimes.size(); ++idx)
        {
            const UInt32 & primeFactor = inPrecedingPrimes[idx];

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
            }
        }
    }

} // namespace std::vector<UInt32>
