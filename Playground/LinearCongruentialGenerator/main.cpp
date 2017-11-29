#include <iostream>
#include <ctime>
#include <cstdint>


auto seed = time(0);


inline uint64_t next(uint64_t seed)
{
    return seed * 6364136223846793005 + 1442695040888963407;
}


int main()
{
    enum { max = 100 };

    uint64_t seed = time(0);

    for (auto i = 0; i != 100 * max; ++i)
    {
        auto r = next(seed);
        std::cout << r % max << "\n";
        seed = r;
    }
}

