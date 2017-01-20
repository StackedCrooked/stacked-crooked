#ifndef NETWORKING_H
#define NETWORKING_H


#include "Decode.h"
#include <cstdint>
#include <cstring>
#include <array>
#include <arpa/inet.h>


struct MACAddress
{
    MACAddress() :
        mData()
    {
    }


    std::array<uint8_t, 6> mData;
};


struct IPv4Address
{
    IPv4Address() : mData() {}

    IPv4Address(int a, int b, int c, int d)
    {
        mData[0] = a;
        mData[1] = b;
        mData[2] = c;
        mData[3] = d;
    }

    uint32_t toInteger() const
    {
        uint32_t result;
        memcpy(&result, mData.data(), mData.size());
        return result;
    }

    friend bool operator==(IPv4Address lhs, IPv4Address rhs)
    {
        return lhs.toInteger() == rhs.toInteger();
    }

    friend bool operator<(IPv4Address lhs, IPv4Address rhs)
    {
        return lhs.toInteger() < rhs.toInteger();
    }

    friend std::ostream& operator<<(std::ostream& os, IPv4Address ip);

    std::array<uint8_t, 4> mData;
};


struct Net16
{
    Net16() : mValue() {}

    explicit Net16(uint16_t value) :
        mValue(htons(value))
    {
    }

    void operator=(uint16_t value)
    {
        *this = Net16(value);
    }

    uint16_t hostValue() const { return ntohs(mValue); }

    friend bool operator==(Net16 lhs, Net16 rhs) { return lhs.mValue == rhs.mValue; }
    friend bool operator<(Net16 lhs, Net16 rhs) { return lhs.hostValue() < rhs.hostValue(); }

    uint16_t mValue;
};


struct EthernetHeader
{
    static EthernetHeader Create()
    {
        auto result = EthernetHeader();
        result.mEtherType = Net16(0x0800);
        return result;
    }

    MACAddress mDestination;
    MACAddress mSource;
    Net16 mEtherType;
};


enum class ProtocolId : uint8_t
{
    UDP = 17
};


struct IPv4Header
{
    static IPv4Header Create(ProtocolId protocol, IPv4Address src_ip, IPv4Address dst_ip)
    {
        auto result = IPv4Header();
        result.mProtocolId = protocol;
        result.mSourceIP = src_ip;
        result.mDestinationIP = dst_ip;
        return result;
    }

    uint8_t mVersionAndIHL = (4u << 4) | 5u;
    uint8_t mTypeOfService;
    Net16 mTotalLength{1514};
    Net16 mIdentification;
    Net16 mFlagsAndFragmentOffset;
    uint8_t mTTL = 255;
    ProtocolId mProtocolId;
    uint16_t mChecksum;
    IPv4Address mSourceIP;
    IPv4Address mDestinationIP;
};


struct UDPHeader
{
    static UDPHeader Create(uint16_t src_port, uint16_t dst_port)
    {
        auto result = UDPHeader();
        result.mSourcePort = src_port;
        result.mDestinationPort = dst_port;
        return result;
    }

    Net16 mSourcePort{0};
    Net16 mDestinationPort{0};
    Net16 mLength{0};
    Net16 mChecksum{0};
};


#endif // NETWORKING_H
