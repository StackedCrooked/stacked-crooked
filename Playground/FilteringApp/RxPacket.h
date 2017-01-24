#pragma once


#include <cstdint>


struct RxPacket
{
    RxPacket() :
        mData(),
        mSize(),
        mBBInterfaceId()
    {
    }

    RxPacket(const uint8_t* data, uint16_t size, uint8_t interfaceId) :
        mData(data),
        mSize(size),
        mBBInterfaceId(interfaceId)
    {
    }

    const uint8_t* data() const { return mData; }
    uint32_t size() const { return mSize; }

    const uint8_t* mData;
    uint16_t mSize;
    uint8_t mBBInterfaceId;
};
