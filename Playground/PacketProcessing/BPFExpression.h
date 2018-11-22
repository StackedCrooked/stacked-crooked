#ifndef BPFEXPRESSION_H
#define BPFEXPRESSION_H


#include "Networking.h"
#include <cstdint>
#include <string>


struct Length
{
    explicit Length(int value) : mValue(value) {}

    std::string toString() const;

    int mValue;
};


struct L3Type
{
    explicit L3Type(EtherType ethertype) : mValue(static_cast<uint16_t>(ethertype)) { }

    std::string toString() const;

    Net16 mValue;
};


struct L4Type
{
    explicit L4Type(ProtocolId protocolId) : mValue(protocolId) { }

    std::string toString() const;

    ProtocolId mValue;
};


struct SourceIPv4
{
    explicit SourceIPv4(IPv4Address ip) : mValue(ip) {}

    std::string toString() const;

    IPv4Address mValue;
};


struct DestinationIPv4
{
    explicit DestinationIPv4(IPv4Address ip) : mValue(ip) {}

    std::string toString() const;

    IPv4Address mValue;
};


struct SourceOrDestinationIPv4
{
    explicit SourceOrDestinationIPv4(IPv4Address ip) : mIP(ip) {}

    std::string toString() const;

    IPv4Address mIP;
};


struct SourcePort
{
    explicit SourcePort(uint16_t value) : mValue(value) { }

    std::string toString() const;

    Net16 mValue;
};


struct DestinationPort
{
    explicit DestinationPort(uint16_t value) : mValue(value) { }

    std::string toString() const;

    Net16 mValue;
};


struct SourceOrDestinationPort
{
    explicit SourceOrDestinationPort(uint16_t value) : mValue(value) {}

    std::string toString() const;

    Net16 mValue;
};


#endif // BPFEXPRESSION_H
