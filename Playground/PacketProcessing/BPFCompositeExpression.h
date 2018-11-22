#ifndef BPFCOMPOSITEEXPRESSION_H
#define BPFCOMPOSITEEXPRESSION_H


#include "BPFExpression.h"


enum PacketFlags
{
    PacketFlags_IPv4 = (1 << 0),
    PacketFlags_IPv6 = (1 << 1),
    PacketFlags_UDP  = (1 << 2),
    PacketFlags_TCP  = (1 << 3)
};


enum FilterFlags
{
    FilterFlags_Length  = (1 << 0),
    FilterFlags_L3Type  = (1 << 1),
    FilterFlags_L4Type  = (1 << 2),
    FilterFlags_SrcIPv4 = (1 << 3),
    FilterFlags_DstIPv4 = (1 << 4),
    FilterFlags_SrcPort = (1 << 5),
    FilterFlags_DstPort = (1 << 6),
};


struct BPFCompositeExpression : BPFExpression
{
    BPFCompositeExpression();

    bool hasFlag(int flag) const
    {
        return (mFilterFlags & flag) == flag;
    }

    void add(BPFExpression& expr);

    void add_impl(Length len)
    {
        mLength = len.mValue;
        mFilterFlags |= FilterFlags_Length;
    }

    void add_impl(BPFCompositeExpression& rhs)
    {
        mFilterFlags |= rhs.mFilterFlags;
        if (rhs.hasFlag(FilterFlags_Length)) { mLength = rhs.mLength; }
        if (rhs.hasFlag(FilterFlags_SrcIPv4)) { mSourceIP = rhs.mSourceIP; }
        if (rhs.hasFlag(FilterFlags_DstIPv4)) { mDestinationIP= rhs.mDestinationIP; }
        if (rhs.hasFlag(FilterFlags_SrcPort)) { mSourcePort = rhs.mSourcePort; }
        if (rhs.hasFlag(FilterFlags_DstPort)) { mDestinationPort = rhs.mDestinationPort; }
    }

    void add_impl(L3Type l3type)
    {
        switch (static_cast<EtherType>(l3type.mValue.hostValue()))
        {
            case EtherType::IPv4:
            {
                mPacketFlags |= PacketFlags_IPv4;
                mFilterFlags |= FilterFlags_L3Type;
                return;
            }
            case EtherType::IPv6:
            {
                mPacketFlags |= PacketFlags_IPv6;
                mFilterFlags |= FilterFlags_L3Type;
                return;
            }
        }

        throw std::runtime_error("Invalid ethertype: " + std::to_string(l3type.mValue.hostValue()));
    }

    void add_impl(L4Type l4type)
    {
        switch (l4type.mValue)
        {
            case ProtocolId::TCP:
            {
                mPacketFlags |= PacketFlags_TCP;
                mFilterFlags |= FilterFlags_L4Type;
                return;
            }
            case ProtocolId::UDP:
            {
                mPacketFlags |= PacketFlags_UDP;
                mFilterFlags |= FilterFlags_L4Type;
                return;
            }
        }

        throw std::runtime_error("Invalid ProtocolId: " + std::to_string(static_cast<int>(l4type.mValue)));
    }

    void add_impl(SourceIPv4 src_ip)
    {
        mFilterFlags |= (FilterFlags_L3Type | FilterFlags_SrcIPv4);
        mSourceIP = src_ip.mValue;
    }

    void add_impl(DestinationIPv4 dst_ip)
    {
        mFilterFlags |= (FilterFlags_L3Type | FilterFlags_DstIPv4);
        mDestinationIP = dst_ip.mValue;
    }

    void add_impl(SourceOrDestinationIPv4)
    {
        throw std::runtime_error("TODO: SourceOrDestinationIPv4");
    }

    void add_impl(SourcePort src_port)
    {
        mFilterFlags |= FilterFlags_SrcPort;
        mSourcePort = src_port.mValue;
    }

    void add_impl(DestinationPort dst_port)
    {
        mFilterFlags |= FilterFlags_DstPort;
        mDestinationPort = dst_port.mValue;
    }

    void add_impl(SourceOrDestinationPort)
    {
        throw std::runtime_error(__FUNCTION__);
    }

    bool match_impl(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l4_offset) const override final
    {
        if (mFilterFlags == 0)
        {
            return true;
        }

        if (mFilterFlags & FilterFlags_Length)
        {
            if (size != mLength)
            {
                return false;
            }
        }

        // if (mFlags & FilterFlags_IPv4)
        // if (mFlags & FilterFlags_IPv6)

        if (mFilterFlags & FilterFlags_SrcIPv4)
        {
            if (mSourceIP != Decode<IPv4Header>(data + l3_offset).mSourceIP)
            {
                return false;
            }
        }

        if (mFilterFlags & FilterFlags_DstIPv4)
        {
            if (mDestinationIP != Decode<IPv4Header>(data + l3_offset).mDestinationIP)
            {
                return false;
            }
        }

        if (mFilterFlags & FilterFlags_SrcPort)
        {
            if (mSourcePort != Decode<TCPHeader>(data + l4_offset).mSourcePort)
            {
                return false;
            }
        }

        if (mFilterFlags & FilterFlags_DstPort)
        {
            if (mDestinationPort != Decode<TCPHeader>(data + l4_offset).mDestinationPort)
            {
                return false;
            }
        }

        return true;
    }

    std::string toStringImpl() const override final;


    uint16_t mFilterFlags = 0;
    uint16_t mPacketFlags = 0;
    uint16_t mLength = 0;
    EtherType mEtherType{};
    IPv4Address mSourceIP{};
    IPv4Address mDestinationIP{};
    Net16 mSourcePort{};
    Net16 mDestinationPort{};
};



#endif // BPFCOMPOSITEEXPRESSION_H
