#include "RxPacket.h"
#include <iostream>


RxPacket::Packets RxPacket::sPackets __attribute__((aligned(2048))) {};
RxPacket::Sizes   RxPacket::sSizes   __attribute__((aligned(512)))  {};
RxPacket::VlanIds RxPacket::sVlanIds __attribute__((aligned(128)))  {};

uint32_t RxPacket::sIds = 0;


RxPacket::RxPacket(const uint8_t* data, uint16_t size, uint8_t interfaceId) :
    mId(sIds++)
{

    std::cout << "sPackets address: " << intptr_t(&sPackets) << std::endl;
    std::cout << "sVlanIds address: " << intptr_t(&sVlanIds) << std::endl;
    std::cout << "sSizes address  : " << intptr_t(&sSizes) << std::endl;

    assert(intptr_t(&sPackets) % 2048 == 0);
    assert(intptr_t(&sVlanIds) % 512  == 0);
    assert(intptr_t(&sSizes  ) % 128  == 0);

    std::cout << "mId=" << mId << std::endl;
    assert(mId < 32);
    sPackets.mArray[mId] = data;
    sSizes[mId] = size;
    sVlanIds[mId] = interfaceId;
}
