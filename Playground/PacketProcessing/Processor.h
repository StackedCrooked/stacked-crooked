#pragma once


#include "Filter.h"
#include <array>
#include <cstdint>


struct MAC : std::array<uint8_t, 6>
{
};


struct IPv4Address : std::array<uint8_t, 4>
{
    IPv4Address() = default;
    IPv4Address(int a, int b, int c, int d)
    {
        (*this)[0] = a;
        (*this)[1] = b;
        (*this)[2] = c;
        (*this)[3] = d;
    }

    uint32_t toInteger() const
    {
        uint32_t result;
        memcpy(&result, data(), size());
        return result;
    }
};


struct EthernetHeader
{
    MAC mDestination;
    MAC mSource;
    uint16_t mEtherType;
};


struct IPv4Header
{
    std::array<uint8_t, 12> mStuff;
    IPv4Address mSource;
    IPv4Address mDestination;
};


struct UDPHeader
{
    uint16_t mSourcePort;
    uint16_t mDestinationPort;
};


struct PacketHeader
{
    PacketHeader() = default;

    PacketHeader(IPv4Address src, IPv4Address dst, uint16_t src_port, uint16_t dst_port)
    {
        mIPv4Header.mSource = src;
        mIPv4Header.mDestination = dst;
        mNetworkHeader.mSourcePort = src_port;
        mNetworkHeader.mDestinationPort = dst_port;
    }

    PacketHeader(uint16_t src_port, uint16_t dst_port)
    {
        mNetworkHeader.mSourcePort = src_port;
        mNetworkHeader.mDestinationPort = dst_port;
    }

    std::size_t hash() const
    {
        std::size_t result = 0;
        boost::hash_combine(result, mIPv4Header.mSource.toInteger());
        boost::hash_combine(result, mIPv4Header.mDestination.toInteger());
        const void* p = &mNetworkHeader;
        boost::hash_combine(result, static_cast<const uint32_t*>(p)[0]);
        return result;
    }

    uint8_t* data() { return static_cast<uint8_t*>(static_cast<void*>(this)); }
    uint8_t* begin() { return data(); }
    uint8_t* end() { return data() + sizeof(*this); }

    const uint8_t* data() const { return static_cast<const uint8_t*>(static_cast<const void*>(this)); }
    const uint8_t* begin() const { return data(); }
    const uint8_t* end() const { return data() + sizeof(*this); }

    EthernetHeader mEthernetHeader = EthernetHeader();
    IPv4Header mIPv4Header = IPv4Header();
    UDPHeader mNetworkHeader = UDPHeader();
};


struct Packet
{
    const uint8_t* mData;
    uint16_t mSize;
};


struct Segment
{
    enum
    {
        max_packets = 65536,
        max_bytes   = max_packets * 64
    };

    std::array<uint16_t, max_packets> mPacketOffsets;
    std::array<uint8_t, max_bytes> mPacketData;
};


struct Processor
{
    Processor() = default;

    Processor(IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port)
    {
        mFilter[0] = source_ip.toInteger();
        mFilter[1] = target_ip.toInteger();

        auto u16 = reinterpret_cast<uint16_t*>(&mFilter[2]);
        u16[0] = src_port;
        u16[1] = dst_port;
    }


    std::size_t hash() const
    {
        std::size_t result = 0;
        boost::hash_combine(result, mFilter[0]);
        boost::hash_combine(result, mFilter[1]);
        boost::hash_combine(result, mFilter[2]);
        return result;
    }


    void process()
    {
        mProcessed++;
    }

    bool match(const uint8_t* frame_bytes)
    {
        enum
        {
            tuple_offset = sizeof(EthernetHeader) + sizeof(IPv4Header) - sizeof(IPv4Address) - sizeof(IPv4Address)
        };

        mChecked++;
        auto tuple_bytes = frame_bytes + tuple_offset;

        auto filter = (uint32_t*)tuple_bytes;
        return (filter[2] == mFilter[2])
             & (filter[1] == mFilter[1])
             & (filter[0] == mFilter[0]);
    }


    // src_ip, dst_ip, src_and_dst_ports
    using Filter = std::array<uint32_t, 3>;

    Filter mFilter = Filter();
    uint64_t mProcessed = 0;
    uint64_t mChecked = 0;
};
