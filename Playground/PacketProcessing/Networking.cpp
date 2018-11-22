#include "Networking.h"
#include <cxxabi.h>
#include <ostream>
#include <sstream>


std::ostream& operator<<(std::ostream& os, IPv4Address ip)
{
    return os << static_cast<int>(ip[0]) << '.'
              << static_cast<int>(ip[1]) << '.'
              << static_cast<int>(ip[2]) << '.'
              << static_cast<int>(ip[3]);
}


std::ostream& operator<<(std::ostream& os, Net16 net16)
{
    return os << net16.hostValue();
}


std::ostream& operator<<(std::ostream& os, Net32 net32)
{
    return os << net32.hostValue();
}

static_assert(std::is_trivially_copyable<Net16>::value, "");
static_assert(std::is_trivially_copyable<MACAddress>::value, "");
static_assert(std::is_trivially_copyable<IPv4Address>::value, "");
static_assert(std::is_trivially_copyable<EthernetHeader>::value, "");
static_assert(std::is_trivially_copyable<IPv4Header>::value, "");
static_assert(std::is_trivially_copyable<TCPHeader>::value, "");


static_assert(sizeof(IPv4Header) == 20, "");
static_assert(sizeof(TCPHeader) == 20, "");


bool IPv4Address::try_parse(const char* s)
{
    int parts[4];

    auto n = sscanf(s, "%d%*[.]%d%*[.]%d%*[.]%d", &parts[0], &parts[1], &parts[2], &parts[3]);

    auto check = [&](int i) {
        return parts[i] >= 0 && parts[i] <= 255;
    };

    if (n == 4 && check(0) && check(1) && check(2) && check(3))
    {
        auto bytes = data();
        bytes[0] = parts[0];
        bytes[1] = parts[1];
        bytes[2] = parts[2];
        bytes[3] = parts[3];
        return true;
    }

    return false;
}


std::string IPv4Address::toString() const
{
    std::stringstream ss;
    auto b = data();
    ss << int(b[0]) << '.' << int(b[1]) << '.' << int(b[2]) << '.' << int(b[3]);
    return ss.str();
}
