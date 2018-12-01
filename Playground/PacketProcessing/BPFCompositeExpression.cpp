#include "BPFCompositeExpression.h"
#include <iostream>
#include <sstream>


BPFCompositeExpression::BPFCompositeExpression()
{
}


void BPFCompositeExpression::add_impl(UDPPayload udp_payload)
{
    mFilterFlags |= FilterFlags_UDPPayload;
    mUDPPayloadOffset = udp_payload.mOffset;
    mUDPPayloadSize = udp_payload.mSize;
    mUDPPayloadValue = udp_payload.mValue;
}


bool BPFCompositeExpression::match(const uint8_t* data, uint32_t size, uint32_t l3_offset, uint32_t l4_offset) const
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

    if (mFilterFlags & FilterFlags_UDPPayload)
    {
        auto payload_index = l4_offset + mUDPPayloadOffset;
        if (payload_index + mUDPPayloadSize >= size)
        {
            return false;
        }

        if (data[payload_index] != mUDPPayloadValue.hostValue())
        {
            return false;
        }
    }

    return true;
}


std::string BPFCompositeExpression::toString() const
{
    std::stringstream ss;

    int count = 0;

    if (hasFlag(FilterFlags_L3Type )) { ss << (count == 0 ? "" : " and ") << ((mPacketFlags & PacketFlags_IPv4) ? "ip" : (mPacketFlags & PacketFlags_IPv6) ? "ip6" : "(NO IP?)") ; count++ ; }
    if (hasFlag(FilterFlags_SrcIPv4)) { ss << (count == 0 ? "" : " and ") << "ip src " << mSourceIP.toString()                  ; count++ ; }
    if (hasFlag(FilterFlags_DstIPv4)) { ss << (count == 0 ? "" : " and ") << "ip dst " << mSourceIP.toString()                  ; count++ ; }
    if (hasFlag(FilterFlags_L4Type )) { ss << (count == 0 ? "" : " and ") << ((mPacketFlags & PacketFlags_UDP) ? "udp" : (mPacketFlags & PacketFlags_TCP) ? "tcp" : "(NO L4)") ; count++ ; }
    if (hasFlag(FilterFlags_SrcPort)) { ss << (count == 0 ? "" : " and ") << "src port " << mSourcePort                         ; count++ ; }
    if (hasFlag(FilterFlags_DstPort)) { ss << (count == 0 ? "" : " and ") << "dst port " << mSourcePort                         ; count++ ; }
    if (hasFlag(FilterFlags_Length )) { ss << (count == 0 ? "" : " and ") << "len=" << mLength                                  ; count++ ; }
    if (hasFlag(FilterFlags_UDPPayload)) { ss << (count == 0 ? "" : " and ") << " udp[" << mUDPPayloadOffset << ":" << mUDPPayloadSize << "]=" << mUDPPayloadValue; count++; }

    return ss.str();
}
