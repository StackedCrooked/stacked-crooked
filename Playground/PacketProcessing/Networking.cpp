#include "Networking.h"
#include <ostream>


bool operator==(IPv4Address lhs, IPv4Address rhs)
{
    return lhs.toInteger() == rhs.toInteger();
}

std::ostream& operator<<(std::ostream& os, IPv4Address ip)
{
    return os << static_cast<int>(ip.mData[0]) << '.'
              << static_cast<int>(ip.mData[1]) << '.'
              << static_cast<int>(ip.mData[2]) << '.'
              << static_cast<int>(ip.mData[3]);
}
