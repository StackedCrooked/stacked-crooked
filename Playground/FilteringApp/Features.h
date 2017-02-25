#pragma once


#include <iostream>


#define FEATURES_ENABLE_STATS       1
#define FEATURES_ENABLE_MAC_CHECK   1
#define FEATURES_ENABLE_IP_CHECK    1
#define FEATURES_ENABLE_UDP         1
#define FEATURES_ENABLE_TCP         1


namespace Features {


enum
{
    enable_stats     = (1 << 0) * FEATURES_ENABLE_STATS,
    enable_mac_check = (1 << 1) * FEATURES_ENABLE_MAC_CHECK,
    enable_ip_check  = (1 << 2) * FEATURES_ENABLE_IP_CHECK,
    enable_udp       = (1 << 3) * FEATURES_ENABLE_UDP,
    enable_tcp       = (1 << 4) * FEATURES_ENABLE_TCP
};


} // namespace Features


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
