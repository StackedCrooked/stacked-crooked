#include <iostream>
#include <map>

unsigned GetFibo(unsigned n)
{
    if (n == 0)
    {
        return 1;
    }

    if (n == 1)
    {
        return 2;
    }

    typedef std::map<unsigned, unsigned> Cache;
    static Cache fCache;
    Cache::iterator it = fCache.find(n);
    if (it != fCache.end())
    {
        return it->second;
    }
    unsigned result = GetFibo(n - 1) + GetFibo(n - 2);
    fCache.insert(std::make_pair(n, result));
    return result;
}

int main()
{
    static const unsigned cFourMillion = 4 * 1000 * 1000;
    unsigned sum = 0;
    int i = 0;
    for (;;)
    {
        unsigned fibo = GetFibo(i++);
        if (fibo >= cFourMillion)
        {
            break;
        }

        if (fibo % 2 == 0)
        {
            sum += fibo;
        }
    }
    std::cout << sum << std::endl;
}
