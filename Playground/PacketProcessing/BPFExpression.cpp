#include "BPFExpression.h"


std::string Length::toString() const
{
    return "len=" + std::to_string(mValue);
}


std::string L3Type::toString() const
{
    switch (EtherType(mValue.hostValue()))
    {
        case EtherType::IPv4: return "ip";
        case EtherType::IPv6: return "ip6";
    }
    return "EtherType(" + std::to_string(static_cast<int>(mValue.hostValue())) + ")";
}


std::string L4Type::toString() const
{
    switch (mValue)
    {
        case ProtocolId::TCP: return "tcp";
        case ProtocolId::UDP: return "udp";
    }
    return "ProtocolId(" + std::to_string(static_cast<int>(mValue)) + ")";
}


std::string SourceIPv4::toString() const
{
    return "src ip " + mValue.toString();
}


std::string DestinationIPv4::toString() const
{
    return "dst ip " + mValue.toString();
}


std::string SourceOrDestinationIPv4::toString() const
{
    return "src or dst ip " + mIP.toString();
}


std::string SourcePort::toString() const
{
    return "src port " + std::to_string(mValue.hostValue());
}


std::string DestinationPort::toString() const
{
    return "dst port " + std::to_string(mValue.hostValue());
}


std::string SourceOrDestinationPort::toString() const
{
    return "src or dst port " + std::to_string(mValue.hostValue());
}


UDPPayload::UDPPayload(int offset, int size, uint32_t value) :
    mOffset(offset),
    mSize(size),
    mValue(value)
{
}


std::string UDPPayload::toString() const
{
    return "udp["
         + std::to_string(mOffset)
         + ":"
         + std::to_string(mSize)
         + "]="
         + std::to_string(mValue.hostValue());
}
