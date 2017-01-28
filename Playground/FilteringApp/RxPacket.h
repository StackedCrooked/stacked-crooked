#pragma once


#include <array>
#include <cassert>
#include <cstdint>


struct RxPacket
{
    RxPacket() :
        mId(sIds++)
    {
    }

    RxPacket(const uint8_t* data, uint16_t size, uint8_t interfaceId);

    const uint8_t* data() const{ return sPackets.mArray[mId]; }
    uint32_t size() const { return sSizes[mId]; }
    uint32_t vlanId() const { return sVlanIds[mId]; }

    uint8_t operator[](uint32_t index) const { return data()[index]; }

private:
    using ConstBytePtr = const uint8_t*;
    enum { storage_size = 256 };

    struct Packets { ConstBytePtr mArray[storage_size]; };

    using Sizes = std::array<uint16_t, storage_size>;
    using VlanIds =  std::array<uint8_t, storage_size>;

    static_assert(sizeof(RxPacket::Packets) == 2048, "");
    static_assert(sizeof(RxPacket::Sizes)   == 512, "");
    static_assert(sizeof(RxPacket::VlanIds) == 256 , "");

    static Packets sPackets;
    static Sizes sSizes;
    static VlanIds sVlanIds;
    static uint32_t sIds;

private:
    uint32_t mId;
};
