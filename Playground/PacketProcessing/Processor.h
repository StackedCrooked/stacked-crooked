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


struct Header
{
    Header() = default;

    Header(IPv4Address src, IPv4Address dst, uint16_t src_port, uint16_t dst_port)
    {
        mIPv4Header.mSource = src;
        mIPv4Header.mDestination = dst;
        mNetworkHeader.mSourcePort = src_port;
        mNetworkHeader.mDestinationPort = dst_port;
    }

    Header(uint16_t src_port, uint16_t dst_port)
    {
        mNetworkHeader.mSourcePort = src_port;
        mNetworkHeader.mDestinationPort = dst_port;
    }

    std::size_t calculate_hash() const
    {
        std::size_t result = 0;
        boost::hash_combine(result, mIPv4Header.mSource.toInteger());
        boost::hash_combine(result, mIPv4Header.mDestination.toInteger());
        boost::hash_combine(result, mNetworkHeader.mSourcePort);
        boost::hash_combine(result, mNetworkHeader.mDestinationPort);
        return result;
    }

    uint8_t* data() { return static_cast<uint8_t*>(static_cast<void*>(this)); }
    uint8_t* begin() { return data(); }
    uint8_t* end() { return data() + sizeof(*this); }

    const uint8_t* data() const { return static_cast<const uint8_t*>(static_cast<const void*>(this)); }
    const uint8_t* begin() const { return data(); }
    const uint8_t* end() const { return data() + sizeof(*this); }

    std::size_t size() const { return sizeof(*this); }

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
        enum : unsigned
        {
            tuple_offset = sizeof(EthernetHeader) + sizeof(IPv4Header) - 2 * sizeof(IPv4Address)
        };


        //std::cout << "mHash=" << mHash;

        boost::hash_combine(mHash, source_ip.toInteger());
        //std::cout << " " << mHash;

        boost::hash_combine(mHash, target_ip.toInteger());
        //std::cout << " " << mHash;

        boost::hash_combine(mHash, src_port);
        //std::cout << " " << mHash;

        boost::hash_combine(mHash, dst_port);
        //std::cout << " " << mHash << std::endl;

        unsigned offset = tuple_offset;

        mFilter.add(source_ip.toInteger(), offset);
        offset += sizeof(source_ip);

        mFilter.add(target_ip.toInteger(), offset);
        offset += sizeof(target_ip);

        mFilter.add(src_port, offset);
        offset += sizeof(src_port);

        mFilter.add(dst_port, offset);
    }

    std::size_t hash() const
    {
        return mHash;
    }

    bool process(std::size_t hash, const uint8_t* frame_bytes, int len)
    {
        if (mHash != hash)
        {
            return false;
        }

        mHashesOk++;

        if (!do_process(frame_bytes, len))
        {
            return false;
        }

        mProcessed++;
        return true;
    }

    std::size_t getMatches() const { return mProcessed; }
    std::size_t getHashMatches() const { return mHashesOk; }

    bool do_process(const uint8_t* frame_bytes, int len)
    {
        return mFilter.match(frame_bytes, len);
    }


private:
    std::size_t mHash = 0 ;
    uint64_t mHashesOk = 0;
    Filter mFilter;
    uint64_t mProcessed = 0;
};
