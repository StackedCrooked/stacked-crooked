#pragma once


#include <cstdint>
#include <cstring>



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
        uint64_t result = 0;
        memcpy(&result, data(), size());
        return result;
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

    typedef uint8_t Array[6];
    Array mArray;
};


