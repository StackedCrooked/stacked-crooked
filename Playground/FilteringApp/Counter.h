#pragma once


#include "Features.h"
#include <cstdint>
#include <iostream>


struct Counter
{
    Counter(uint64_t value) : mValue(value) {}

    Counter operator++(int)
    {
        if (Features::enable_stats)
        {
            mValue++;
        }
        return *this;
    }

    Counter& operator+=(uint64_t n)
    {
        if (Features::enable_stats)
        {
            mValue += n;
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Counter& counter)
    {
        return os << counter.mValue;
    }

    uint64_t mValue = 0;
};
