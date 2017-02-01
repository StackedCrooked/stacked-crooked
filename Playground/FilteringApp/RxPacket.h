#pragma once


#include <cassert>
#include <cstdint>


struct RxPacket
{
    RxPacket() :
        mData(),
        mSize(),
        mVlanId()
    {
    }

    RxPacket(const uint8_t* data, uint16_t size, uint8_t interfaceId) :
        mData(data),
        mSize(size),
        mVlanId(interfaceId)
    {
    }

    const uint8_t* data() const{ return mData; }
    uint32_t size() const { return mSize; }

    uint8_t operator[](uint32_t index) const { return mData[index]; }

    const uint8_t* mData;
    uint16_t mSize;
    uint8_t mVlanId;
};
