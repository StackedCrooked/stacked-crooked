#include "BPFCompositeExpression.h"
#include <iostream>
#include <sstream>


BPFCompositeExpression::BPFCompositeExpression()
{
}


void BPFCompositeExpression::merge_and(BPFCompositeExpression& rhs)
{
    // TODO: FR: We must check for conflicts (in which case the filter always returns false)

    mFilterFlags |= rhs.mFilterFlags;
    mPacketFlags |= rhs.mPacketFlags;

    if (rhs.has_flag(FilterFlags_Length))
    {
        mLength = rhs.mLength;
    }

    if (rhs.has_flag(FilterFlags_SrcIPv4))
    {
        mPacketFlags |= PacketFlags_IPv4;
        mSourceIP = rhs.mSourceIP;
    }

    if (rhs.has_flag(FilterFlags_DstIPv4))
    {
        mPacketFlags |= PacketFlags_IPv4;
        mDestinationIP= rhs.mDestinationIP;
    }

    if (rhs.has_flag(FilterFlags_SrcPort))
    {
        mSourcePort = rhs.mSourcePort;
    }

    if (rhs.has_flag(FilterFlags_DstPort))
    {
        mDestinationPort = rhs.mDestinationPort;
    }

    if (rhs.has_flag(FilterFlags_UDPPayload))
    {
        mPacketFlags |= PacketFlags_UDP;
        mUDPPayloadOffset = rhs.mUDPPayloadOffset;
        mUDPPayloadSize = rhs.mUDPPayloadSize;
        mUDPPayloadValue = rhs.mUDPPayloadValue;
    }
}


bool BPFCompositeExpression::merge_or(BPFCompositeExpression& rhs)
{
    if (mFilterFlags != rhs.mFilterFlags)
    {
        return false;
    }

    if (mPacketFlags != rhs.mPacketFlags)
    {
        return false;
    }

    if (has_flag(FilterFlags_SrcIPv4) && mSourceIP != rhs.mSourceIP)
    {
        return false;
    }

    if (has_flag(FilterFlags_DstIPv4) && mDestinationIP != rhs.mDestinationIP)
    {
        return false;
    }

    if (has_flag(FilterFlags_SrcPort) && mSourcePort != rhs.mSourcePort)
    {
        return false;
    }

    if (has_flag(FilterFlags_DstPort) && mDestinationPort != rhs.mDestinationPort)
    {
        return false;
    }

    if (has_flag(FilterFlags_UDPPayload)
            && mUDPPayloadOffset != rhs.mUDPPayloadOffset
            && mUDPPayloadSize   != rhs.mUDPPayloadSize
            && mUDPPayloadValue  != rhs.mUDPPayloadValue)
    {
        return false;
    }

    if (has_flag(FilterFlags_Length) && !match_length(rhs))
    {
        mLengths.push_back(rhs.mLength);
        mLengths.insert(mLengths.end(), rhs.mLengths.begin(), rhs.mLengths.end());
        return true;
    }

    // Filters are identical.
    return true;
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
        if (!match_length(size))
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

    if (has_flag(FilterFlags_L3Type))
    {
        ss << (count == 0 ? "" : " and ") << ((mPacketFlags & PacketFlags_IPv4) ? "ip" : (mPacketFlags & PacketFlags_IPv6) ? "ip6" : "(NO IP?)");
        count++;
    }

    if (has_flag(FilterFlags_SrcIPv4))
    {
        ss << (count == 0 ? "" : " and ") << "ip src " << mSourceIP.toString();
        count++;
    }

    if (has_flag(FilterFlags_DstIPv4))
    {
        ss << (count == 0 ? "" : " and ") << "ip dst " << mSourceIP.toString();
        count++;
    }

    if (has_flag(FilterFlags_L4Type))
    {
        ss << (count == 0 ? "" : " and ") << ((mPacketFlags & PacketFlags_UDP) ? "udp" : (mPacketFlags & PacketFlags_TCP) ? "tcp" : "(NO L4)");
        count++;
    }

    if (has_flag(FilterFlags_SrcPort))
    {
        ss << (count == 0 ? "" : " and ") << "src port " << mSourcePort;
        count++;
    }

    if (has_flag(FilterFlags_DstPort))
    {
        ss << (count == 0 ? "" : " and ") << "dst port " << mSourcePort;
        count++;
    }

    if (has_flag(FilterFlags_Length))
    {
        ss << (count == 0 ? "" : " and ");
        if (mLengths.empty())
        {
            ss << "len=" << mLength;
        }
        else
        {
            ss << "(len=" << mLength;
            for (auto value : mLengths)
            {
                ss << " or len=" << value;
            }
            ss << ")";
        }
        count++;
    }

    if (has_flag(FilterFlags_UDPPayload))
    {
        ss << (count == 0 ? "" : " and ") << " udp[" << mUDPPayloadOffset << ":" << mUDPPayloadSize << "]=" << mUDPPayloadValue;
        count++;
    }

    return ss.str();
}
