#ifndef BPFEXPRESSION_H
#define BPFEXPRESSION_H


#include "Networking.h"
#include <cstdint>
#include <string>


struct BPFExpression
{
    virtual ~BPFExpression() {}

    bool match(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l4_offset) const
    {
        return match_impl(data, size, l3_offset, l4_offset);
    }

    std::string toString() const
    {
        return toStringImpl();
    }

private:
    virtual bool match_impl(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l4_offset) const = 0;
    virtual std::string toStringImpl() const = 0;

};


struct L3Type : BPFExpression
{
    explicit L3Type(EtherType ethertype) :
        mValue(static_cast<uint16_t>(ethertype))
    {
    }

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t /*l3_offset*/, uint32_t /*l4_offset*/) const override final
    {
        return Decode<EthernetHeader>(data).mEtherType == mValue;
    }

    std::string toStringImpl() const override final
    {
        switch (EtherType(mValue.hostValue()))
        {
            case EtherType::IPv4: return "ip";
            case EtherType::IPv6: return "ip6";
            case EtherType::VLAN: return "vlan";
            case EtherType::ARP: return "arp";
        }
        return "(unknown l3)";
    }

    Net16 mValue;
};


struct L4Type : BPFExpression
{
    explicit L4Type(ProtocolId protocolId) :
        mValue(protocolId)
    {
    }

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t l3_offset, uint32_t /*l4_offset*/) const override final
    {
        return Decode<IPv4Header>(data + l3_offset).mProtocol == mValue;
    }

    std::string toStringImpl() const override final
    {
        switch (mValue)
        {
            case ProtocolId::ICMP: return "icmp";
            case ProtocolId::IGMP: return "igmp";
            case ProtocolId::TCP: return "tcp";
            case ProtocolId::UDP: return "udp";
        }
        return "(unknown l3)";
    }


    ProtocolId mValue;
};


struct SourceIPv4 : BPFExpression
{
    explicit SourceIPv4(IPv4Address ip) : mIP(ip) {}

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t l3_offset, uint32_t /*l4_offset*/) const override final
    {
        return Decode<IPv4Header>(data + l3_offset).mSourceIP == mIP;
    }

    std::string toStringImpl() const override final
    {
        return "src ip " + mIP.toString();
    }

    IPv4Address mIP;
};


struct DestinationIPv4 : BPFExpression
{
    explicit DestinationIPv4(IPv4Address ip) : mIP(ip) {}

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t l3_offset, uint32_t /*l4_offset*/) const override final
    {
        return Decode<IPv4Header>(data + l3_offset).mDestinationIP == mIP;
    }

    std::string toStringImpl() const override final
    {
        return "dst ip " + mIP.toString();
    }

    IPv4Address mIP;
};


struct SourceOrDestinationIPv4 : BPFExpression
{
    explicit SourceOrDestinationIPv4(IPv4Address ip) : mIP(ip) {}

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t l3_offset, uint32_t /*l4_offset*/) const override final
    {
        return Decode<IPv4Header>(data + l3_offset).mSourceIP      == mIP
            || Decode<IPv4Header>(data + l3_offset).mDestinationIP == mIP;
    }

    std::string toStringImpl() const override final
    {
        return "src or dst ip " + mIP.toString();
    }

    IPv4Address mIP;
};


struct SourcePort : BPFExpression
{
    explicit SourcePort(uint16_t value) : mValue(value)
    {
    }

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t /*l3_offset*/, uint32_t l4_offset) const override final
    {
        auto src_port = Decode<TCPHeader>(data + l4_offset).mSourcePort;
        return src_port == mValue;
    }

    std::string toStringImpl() const override final
    {
        return "src port " + std::to_string(mValue.hostValue());
    }

    Net16 mValue;
};


struct DestinationPort : BPFExpression
{
    explicit DestinationPort(uint16_t value) : mValue(value)
    {
    }

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t /*l3_offset*/, uint32_t l4_offset) const override final
    {
        return Decode<TCPHeader>(data + l4_offset).mDestinationPort == mValue;
    }

    std::string toStringImpl() const override final
    {
        return "dst port " + std::to_string(mValue.hostValue());
    }

    Net16 mValue;
};


struct SourceOrDestinationPort : BPFExpression
{
    explicit SourceOrDestinationPort(uint16_t value) : mValue(value) {}

    bool match_impl(const uint8_t* data, uint32_t /*size*/, uint32_t /*l3_offset*/, uint32_t l4_offset) const override final
    {
        return Decode<TCPHeader>(data + l4_offset).mSourcePort      == mValue
            || Decode<TCPHeader>(data + l4_offset).mDestinationPort == mValue;
    }

    std::string toStringImpl() const override final
    {
        return "src or dst port " + std::to_string(mValue.hostValue());
    }

    Net16 mValue;
};


#endif // BPFEXPRESSION_H
