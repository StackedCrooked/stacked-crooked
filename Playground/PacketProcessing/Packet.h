#ifndef PACKET_H
#define PACKET_H


#include "Networking.h"
#include "PCAPWriter.h"
#include <cstdint>


struct Packet
{
    Packet(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    const uint8_t* data() const { return mPayload.data(); }

    uint32_t size() const { return mPayload.size(); }

private:
    std::array<uint8_t, 1536> mPayload;
};


#endif // PACKET_H
