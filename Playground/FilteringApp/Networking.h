#ifndef NETWORKING_H
#define NETWORKING_H


#include "Decode.h"
#include "MACAddress.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <array>
#include <arpa/inet.h>


struct IPv4Address
{
    static IPv4Address Create(int i)
    {
        IPv4Address result;
        result.mData[0] = 1;
        result.mData[1] = 1;
        result.mData[2] = 1;
        result.mData[3] = i;
        return result;
    }

    IPv4Address() : mData() {}

    IPv4Address(int a, int b, int c, int d)
    {
        mData[0] = a;
        mData[1] = b;
        mData[2] = c;
        mData[3] = d;
    }

    uint8_t operator[](uint32_t i) const
    {
        return mData[i];
    }

    bool isBroadcast() const { return *this == IPv4Address(0xFF, 0xFF, 0xFF, 0xFF); }
    bool isMulticast() const { return (mData[0] & 0xF0) == 0xE0; }


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

    friend bool operator!=(IPv4Address lhs, IPv4Address rhs)
    {
        return lhs.toInteger() != rhs.toInteger();
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


struct Net32
{
    Net32() : mValue() {}

    explicit Net32(uint32_t value) :
        mValue(htonl(value))
    {
    }

    void operator=(uint32_t value)
    {
        *this = Net32(value);
    }

    uint32_t hostValue() const { return ntohl(mValue); }

    friend bool operator==(Net32 lhs, Net32 rhs) { return lhs.mValue == rhs.mValue; }
    friend bool operator<(Net32 lhs, Net32 rhs) { return lhs.hostValue() < rhs.hostValue(); }

    uint32_t mValue;
};


struct EthernetHeader
{
    static EthernetHeader Create(MACAddress mac)
    {
        auto result = EthernetHeader();
        result.mDestination = mac;
        result.mEtherType = Net16(0x0800);
        return result;
    }

    MACAddress mDestination;
    MACAddress mSource;
    Net16 mEtherType;
};


enum ProtocolId : uint8_t
{
    ICMP = 1,
    IGMP = 2,
    TCP = 6,
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


struct TCPHeader
{
    static TCPHeader Create(uint16_t src_port, uint16_t dst_port)
    {
        auto result = TCPHeader();
        result.mSourcePort = src_port;
        result.mDestinationPort = dst_port;
        return result;
    }

    Net16 mSourcePort{0};
    Net16 mDestinationPort{0};
    Net32 mSequenceNumber;
    Net32 mAcknowledgmentNumber;
    Net16 mDataOffsetAndFlags;
    Net16 mWindowSize;
    Net16 mCheckSum;
    Net16 mUrgentPointer;
};



#endif // NETWORKING_H
