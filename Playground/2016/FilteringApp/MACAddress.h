#pragma once


#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>



struct MACAddress
{
    const uint8_t& operator[](int idx) const { return mArray[idx]; }
    uint8_t& operator[](int idx) { return mArray[idx]; }

    const uint8_t* data() const { return &mArray[0]; }
    uint8_t* data() { return &mArray[0]; }

    uint32_t size() const { return sizeof(mArray); }

    bool isBroadcast() const
    {
        return *this == BroadcastAddress();
    }

    bool isMulticast() const
    {
        return mArray[0] & 0x01;
    }

    uint64_t toInteger() const
    {
        union
        {
            uint16_t u16[4];
            uint32_t u32[2];
            uint64_t u64;
        };

        u32[0] = Decode<uint32_t>(data());
        u16[2] = Decode<uint16_t>(data() + sizeof(uint32_t));
        u16[3] = 0;

        return u64;
    }

    static MACAddress BroadcastAddress()
    {
        return MACAddress{{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }};
    }

    friend bool operator==(MACAddress lhs, MACAddress rhs)
    {
        auto lhs_u16 = reinterpret_cast<const uint16_t*>(lhs.data());
        auto rhs_u16 = reinterpret_cast<const uint16_t*>(rhs.data());
        return (lhs_u16[0] == rhs_u16[0])
            && (lhs_u16[1] == rhs_u16[1])
            && (lhs_u16[2] == rhs_u16[2]);
    }

    friend bool operator!=(MACAddress lhs, MACAddress rhs)
    {
        return !(lhs == rhs);
    }

    friend bool operator<(MACAddress lhs, MACAddress rhs)
    {
        return lhs.toInteger() < rhs.toInteger();
    }

    friend std::ostream& operator<<(std::ostream& os, MACAddress mac)
    {
        return os
            << std::hex << std::setw(2) << std::setfill('0') << (int)mac[0]
            << '-' << std::hex << std::setw(2) << std::setfill('0') << (int)mac[1]
            << '-' << std::hex << std::setw(2) << std::setfill('0') << (int)mac[2]
            << '-' << std::hex << std::setw(2) << std::setfill('0') << (int)mac[3]
            << '-' << std::hex << std::setw(2) << std::setfill('0') << (int)mac[4]
            << '-' << std::hex << std::setw(2) << std::setfill('0') << (int)mac[5]
            << std::dec;
    }

    typedef uint8_t Array[6];
    Array mArray;
};


// LocalMAC stores a MAC inside a uint64_t which allows for fast comparision.
struct LocalMAC
{
    LocalMAC(MACAddress mac) :
        mMAC(mac),
        mPadding(0)
    {
    }

    const uint8_t* data() { return mMAC.data(); }
    std::size_t size() const { return sizeof(mMAC); }

    const uint8_t* storage_data() { return mBytes; }
    std::size_t storage_size() { return 8; }

    // Compare against MAC data inside network packet.
    // This allows us to safely assume the given data
    // buffer has a size >= 8. Which allows us to use
    // a single 64-bit comparison.
    bool equals(const uint8_t* frame_data)
    {
        return mValue == (Decode<uint64_t>(frame_data) & 0x0000FFFFFFFFFFFF);
    }

    bool check_mac(const MACAddress& mac)
    {
        return (mU16[2] == Decode<uint16_t>(mac.data() + 4))
            && (mU32[0] == Decode<uint32_t>(mac.data() + 0));
    }

    union
    {
        uint64_t mValue;
        uint64_t mU64[1];
        uint32_t mU32[2];
        uint16_t mU16[4];
        uint8_t mBytes[8];
        struct
        {
            MACAddress mMAC;
            uint16_t mPadding;
        };
    };
};

