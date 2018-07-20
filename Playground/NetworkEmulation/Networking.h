#ifndef NETWORKING_H
#define NETWORKING_H


#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>
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
    MACAddress() : mData() { }

    explicit MACAddress(uint32_t identifier) : mData()
    {
        mData.back() = identifier;
    }

    const uint8_t* data() const
    {
        return mData.data();
    }

    uint32_t size() const
    {
        return mData.size();
    }

    friend bool operator<(const MACAddress& lhs, const MACAddress& rhs)
    {
        return lhs.mData < rhs.mData;
    }

    friend bool operator==(const MACAddress& lhs, const MACAddress& rhs)
    {
        return lhs.mData == rhs.mData;
    }

    friend bool operator!=(const MACAddress& lhs, const MACAddress& rhs)
    {
        return lhs.mData != rhs.mData;
    }

    friend std::ostream& operator<<(std::ostream & os, const MACAddress& mac)
    {
        return os
            << std::hex
//            << std::setw(2) << std::setfill('0') << int(mac.mData[0]) << ":"
//            << std::setw(2) << std::setfill('0') << int(mac.mData[1]) << ":"
//            << std::setw(2) << std::setfill('0') << int(mac.mData[2]) << ":"
//            << std::setw(2) << std::setfill('0') << int(mac.mData[3]) << ":"
//            << std::setw(2) << std::setfill('0') << int(mac.mData[4]) << ":"
            << std::setw(2) << std::setfill('0') << int(mac.mData[5])
            << std::dec;
    }

    std::array<uint8_t, 6> mData;
};


#endif // NETWORKING_H
