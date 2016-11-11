#ifndef NETWORKING_H
#define NETWORKING_H


#include <cstdint>
#include <cstring>
#include <array>
#include <arpa/inet.h>


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


struct Net16
{
    Net16() = default;

    Net16(uint16_t value) :
        mValue(htons(value))
    {
    }

    void set(uint16_t value)
    {
        *this = Net16(value);
    }

    void operator=(uint16_t value)
    {
        set(value);
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
    Net16 mTotalLength = 1514;
    Net16 mIdentification = 0;
    Net16 mFlagsAndFragmentOffset = 0;
    uint8_t mTTL = 255;
    uint8_t mProtocol = 0;
    uint16_t mChecksum = 0;
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

    Net16 mSourcePort{0};
    Net16 mDestinationPort{0};
    Net16 mSequenceNumber[2];
    Net16 mAcknowledgementNumber[2];
    Net16 mDataOffsetAndFlags = 0;
    Net16 mWindowSize;
    Net16 mChecksum = 0;
    Net16 mUrgentPointer = 0;
};


#endif // NETWORKING_H
