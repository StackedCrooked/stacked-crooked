#ifndef VECTORFILTER_H
#define VECTORFILTER_H


#include "Networking.h"
#include <cstdint>
#include <iostream>
#include <x86intrin.h>
#include "vectorclass/vectori128.h"


using namespace vec;


struct VectorFilter
{
    VectorFilter(ProtocolId protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    bool match(const uint8_t* packet_data, uint32_t /*len*/, uint32_t /*l3_offset*/, uint32_t l4_offset) const
    {
        const auto offset = l4_offset + sizeof(uint16_t) + sizeof(uint16_t) - sizeof(std::array<uint64_t, 2>);
        Vec4ui item;
        item.load(packet_data + offset);

        return !vec::horizontal_or(field_ ^ (item & static_mask_));

    }

private:
    static Vec4ui GetMask()
    {
        uint8_t mask_bytes[16] = {
            0x00, 0xff, 0x00, 0x00, // ttl, protocol and checksum
            0xff, 0xff, 0xff, 0xff, // source ip
            0xff, 0xff, 0xff, 0xff, // destination ip
            0xff, 0xff, 0xff, 0xff  // source and destination ports
        };

        Vec4ui result;
        result.load(&mask_bytes[0]);
        return result;
    }

    Vec4ui field_;
    static Vec4ui static_mask_;
};


#endif // VECTORFILTER_H
