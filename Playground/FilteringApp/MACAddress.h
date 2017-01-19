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
        return lhs.toInteger() == rhs.toInteger();
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


