#include "Networking.h"
#include <ostream>


std::ostream& operator<<(std::ostream& os, IPv4Address ip)
{
    return os << static_cast<int>(ip.mData[0]) << '.'
              << static_cast<int>(ip.mData[1]) << '.'
              << static_cast<int>(ip.mData[2]) << '.'
              << static_cast<int>(ip.mData[3]);
}


static_assert(std::is_trivially_copyable<Net16>::value, "");
static_assert(std::is_trivially_copyable<MACAddress>::value, "");
static_assert(std::is_trivially_copyable<IPv4Address>::value, "");
static_assert(std::is_trivially_copyable<EthernetHeader>::value, "");
static_assert(std::is_trivially_copyable<IPv4Header>::value, "");
static_assert(std::is_trivially_copyable<TCPHeader>::value, "");


static_assert(sizeof(IPv4Header) == 20, "");
static_assert(sizeof(TCPHeader) == 20, "");
