#ifndef NETWORKING_H
#define NETWORKING_H


#include <cstdint>
#include <cstring>
#include <array>
#include <arpa/inet.h>



template<typename T>
inline const T& Decode(const uint8_t* data)
{
    return *reinterpret_cast<const T*>(data);
}


struct Net16
{
    Net16() = default;

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
    friend bool operator!=(Net16 lhs, Net16 rhs) { return lhs.mValue != rhs.mValue; }
    friend bool operator<(Net16 lhs, Net16 rhs) { return lhs.hostValue() < rhs.hostValue(); }
    friend std::ostream& operator<<(std::ostream& os, Net16 net16);

    uint16_t mValue;
};


struct Net32
{
    Net32() = default;

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
    friend bool operator!=(Net32 lhs, Net32 rhs) { return lhs.mValue != rhs.mValue; }
    friend bool operator<(Net32 lhs, Net32 rhs) { return lhs.hostValue() < rhs.hostValue(); }
    friend std::ostream& operator<<(std::ostream& os, Net32 net32);

    uint32_t mValue;
};


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

    bool try_parse(const char* s);

    uint8_t& operator[](std::size_t i) { return data()[i]; }
    const uint8_t& operator[](std::size_t i) const { return data()[i]; }

    uint8_t* data() { return static_cast<uint8_t*>(static_cast<void*>(&mValue)); }
    const uint8_t* data() const { return static_cast<const uint8_t*>(static_cast<const void*>(&mValue)); }

	uint32_t size() const { return sizeof(mValue); }
    uint32_t toInteger() const { return mValue.hostValue(); }
    std::string toString() const;

    friend bool operator==(IPv4Address lhs, IPv4Address rhs)
    { return lhs.mValue == rhs.mValue; }

    friend bool operator!=(IPv4Address lhs, IPv4Address rhs)
    { return lhs.mValue != rhs.mValue; }

    friend bool operator<(IPv4Address lhs, IPv4Address rhs)
    { return lhs.mValue.hostValue() < rhs.mValue.hostValue(); }

    friend std::ostream& operator<<(std::ostream& os, IPv4Address ip);

    Net32 mValue;
};


enum class EtherType : uint16_t
{
    IPv4  = 0x0800,
    IPv6  = 0x86DD
};


struct EthernetHeader
{
    static EthernetHeader Create(EtherType ethertype)
    {
        auto result = EthernetHeader();
        result.mEtherType = Net16(static_cast<uint16_t>(ethertype));
        return result;
    }

    MACAddress mDestination;
    MACAddress mSource;
    Net16 mEtherType;
};


enum class ProtocolId : uint8_t
{
    TCP  = 6,
    UDP  = 17
};


struct IPv4Header
{
    static IPv4Header Create(ProtocolId protocol, IPv4Address src_ip, IPv4Address dst_ip)
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
    ProtocolId mProtocol;
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

    UDPHeader() = default;

    Net16 mSourcePort;
    Net16 mDestinationPort;
    Net16 mChecksum;
    Net16 mLength;
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
