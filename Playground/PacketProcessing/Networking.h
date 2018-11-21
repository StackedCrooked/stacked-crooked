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
    MACAddress() :
        mData()
    {
    }


    std::array<uint8_t, 6> mData;
};


struct IPv4Address
{
    IPv4Address() : mValue() {}

    IPv4Address(int a, int b, int c, int d)
    {
		auto u8 = data();
        u8[0] = a;
        u8[1] = b;
        u8[2] = c;
        u8[3] = d;
    }


    uint8_t& operator[](std::size_t i) { return data()[i]; }
    const uint8_t& operator[](std::size_t i) const { return data()[i]; }

    uint8_t* data() { return static_cast<uint8_t*>(static_cast<void*>(&mValue)); }
    const uint8_t* data() const { return static_cast<const uint8_t*>(static_cast<const void*>(&mValue)); }

	uint32_t size() const { return sizeof(mValue); }
    uint32_t toInteger() const { return mValue; }

    friend bool operator==(IPv4Address lhs, IPv4Address rhs)
    { return lhs.mValue == rhs.mValue; }

    friend bool operator<(IPv4Address lhs, IPv4Address rhs)
    { return lhs.mValue < rhs.mValue; }

    friend std::ostream& operator<<(std::ostream& os, IPv4Address ip);

	uint32_t mValue;
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


static constexpr uint16_t ARP   = 0x0806;
static constexpr uint16_t IPv4  = 0x0800;
static constexpr uint16_t VLAN  = 0x8100;
static constexpr uint16_t IPv6  = 0x86DD;


struct EthernetHeader
{
    static EthernetHeader Create()
    {
        auto result = EthernetHeader();
        result.mEtherType = Net16(IPv4);
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
    uint8_t mTypeOfService;
    Net16 mTotalLength{1514};
    Net16 mIdentification;
    Net16 mFlagsAndFragmentOffset;
    uint8_t mTTL = 255;
    uint8_t mProtocol;
    uint16_t mChecksum;
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
    Net16 mDataOffsetAndFlags;
    Net16 mWindowSize;
    Net16 mChecksum;
    Net16 mUrgentPointer;
};


#endif // NETWORKING_H
