#include "example.h"
#include <algorithm>

int fact(int n) {
    if (n < 0){ /* This should probably return an error, but this is simpler */
        return 0;
    }
    if (n == 0) {
        return 1;
    }
    else {
        /* testing for overflow would be a good idea here */
        return n * fact(n-1);
    }
}

int accumulate(const std::vector<int>& vec)
{
    return std::accumulate(vec.begin(), vec.end(), 0);
}
