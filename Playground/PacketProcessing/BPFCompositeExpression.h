#ifndef BPFCOMPOSITEEXPRESSION_H
#define BPFCOMPOSITEEXPRESSION_H


#include "BPFExpression.h"
#include <algorithm>
#include <vector>


enum PacketFlags
{
    PacketFlags_IPv4 = (1 << 0),
    PacketFlags_IPv6 = (1 << 1),
    PacketFlags_UDP  = (1 << 2),
    PacketFlags_TCP  = (1 << 3)
};


enum FilterFlags
{
    FilterFlags_Length     = (1 << 0),
    FilterFlags_L3Type     = (1 << 1),
    FilterFlags_L4Type     = (1 << 2),
    FilterFlags_SrcIPv4    = (1 << 3),
    FilterFlags_DstIPv4    = (1 << 4),
    FilterFlags_SrcPort    = (1 << 5),
    FilterFlags_DstPort    = (1 << 6),
    FilterFlags_UDPPayload = (1 << 7)
};


struct BPFCompositeExpression
{
    BPFCompositeExpression();

    bool has_flag(int flag) const
    {
        return (mFilterFlags & flag) == flag;
    }

    void add_impl(Length len)
    {
        mLength = len.mValue;
        mFilterFlags |= FilterFlags_Length;
    }

    void merge_and(BPFCompositeExpression& rhs);

    bool merge_or(BPFCompositeExpression& rhs);

    bool match_length(const BPFCompositeExpression& rhs) const
    {
        if (match_length(rhs.mLength))
        {
            return true;
        }

        for (auto length : rhs.mLengths)
        {
            if (match_length(length))
            {
                return true;
            }
        }

        return false;
    }

    bool match_length(uint16_t value) const
    {
        if (value == mLength)
        {
            return true;
        }

        return !mLengths.empty()
            && std::find(mLengths.begin(), mLengths.end(), value) != mLengths.end();
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

    void add_impl(UDPPayload udp_payload);

    bool match(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l4_offset) const;

    std::string toString() const;


    uint16_t mFilterFlags = 0;
    uint16_t mPacketFlags = 0;
    uint16_t mLength = 0;
    IPv4Address mSourceIP{};
    IPv4Address mDestinationIP{};
    Net16 mSourcePort{};
    Net16 mDestinationPort{};
    uint16_t mUDPPayloadOffset = 0;
    uint16_t mUDPPayloadSize = 0;
    Net32 mUDPPayloadValue{};
    std::vector<uint16_t> mLengths;
};



#endif // BPFCOMPOSITEEXPRESSION_H
