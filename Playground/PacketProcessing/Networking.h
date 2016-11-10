#ifndef NETWORKING_H
#define NETWORKING_H


#include <cstdint>
#include <cstring>
#include <array>


template<typename T>
inline T Decode(const uint8_t* data)
{
    auto result = T();
    memcpy(&result, data, sizeof(result));
    return result;
}


struct MACAddress
{
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

    friend std::ostream& operator<<(std::ostream& os, IPv4Address ip);

    std::array<uint8_t, 4> mData;
};


struct EthernetHeader
{
    static EthernetHeader Create()
    {
        auto result = EthernetHeader();
        result.mEtherType = 0x0008;
        return result;
    }

    MACAddress mDestination;
    MACAddress mSource;
    uint16_t mEtherType;
};


struct IPv4Header
{
    static IPv4Header Create(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip)
    {
        auto result = IPv4Header();
        result.mProtocol = protocol;
        result.mSourceIP = src_ip;
        result.mDestinationIP = dst_ip;
        return result;
    }

    uint8_t mVersionAndIHL = (4u << 4) | 5u;
    uint8_t mTypeOfService = 0;
    uint16_t mTotalLength = 1514;
    uint16_t mIdentification = 0;
    uint16_t mFlagsAndFragmentOffset = 0;
    uint8_t mTTL = 255;
    uint8_t mProtocol;
    uint16_t mChecksum =0;
    IPv4Address mSourceIP;
    IPv4Address mDestinationIP;
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

    uint16_t mSourcePort = 0;
    uint16_t mDestinationPort = 0;
    uint16_t mSequenceNumber[2];
    uint16_t mAcknowledgementNumber[2];
    uint16_t mDataOffsetAndFlags = 0;
    uint16_t mWindowSize;
    uint16_t mChecksum = 0;
    uint16_t mUrgentPointer = 0;
};


#endif // NETWORKING_H
