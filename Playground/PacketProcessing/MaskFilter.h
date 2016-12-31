#ifndef MASKFILTER_H
#define MASKFILTER_H


#include "Networking.h"
#include <array>
#include <cstdint>


struct MaskFilter
{
    MaskFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    bool match(const uint8_t* packet_data, uint32_t /*len*/) const
    {
        uint8_t mask_bytes[16] = {
            0x00, 0xff, 0x00, 0x00, // ttl, protocol and checksum
            0xff, 0xff, 0xff, 0xff, // source ip
            0xff, 0xff, 0xff, 0xff, // destination ip
            0xff, 0xff, 0xff, 0xff  // source and destination ports
        };

        auto u64_mask = Decode<std::array<uint64_t, 2>>(&mask_bytes[0]);
        auto u64_data = Decode<std::array<uint64_t, 2>>(packet_data + offset);

        return (mFields[1] == (u64_mask[1] & u64_data[1]))
            && (mFields[0] == (u64_mask[0] & u64_data[0]));
    }

private:
    enum { offset = sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(uint16_t) + sizeof(uint16_t) - sizeof(std::array<uint64_t, 2>) };

    std::array<uint64_t, 2> mFields;
};


#endif // MASKFILTER_H

#if 0


bool match(RxPacket packet)
{
    // points to TTL field in the IP header.
    auto key_data = packet.get_l4() - 2 * sizeof(uint32_t);

    // Get the tuple as an array of 2 uint64_t.
    auto u64_value = Decode<std::array<uint64_t, 2>>(key_data);

    static constexpr uint8_t mask[16] = {
        0x00, 0xFF, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    auto u64_mask = Decode<std::array<uint64_t, 2>>(&mask[0]);


    return (mFields[1] == u64_value[1] & u64_mask[1])
         & (mFields[0] == u64_value[0] & u64_mask[0]);
}

#endif
