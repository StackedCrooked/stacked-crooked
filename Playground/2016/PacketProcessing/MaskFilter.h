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
        auto u64_data = Decode<std::array<uint64_t, 2>>(packet_data + offset);

        return (mFields[1] == (static_mask[1] & u64_data[1]))
            && (mFields[0] == (static_mask[0] & u64_data[0]));
    }

private:
    enum { offset = sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(uint16_t) + sizeof(uint16_t) - sizeof(std::array<uint64_t, 2>) };

    static std::array<uint64_t, 2> GetMask()
    {
        static const uint8_t mask_bytes[16] = {
            0x00, 0xff, 0x00, 0x00, // ttl, protocol and checksum
            0xff, 0xff, 0xff, 0xff, // source ip
            0xff, 0xff, 0xff, 0xff, // destination ip
            0xff, 0xff, 0xff, 0xff  // source and destination ports
        };

        return Decode<std::array<uint64_t, 2>>(&mask_bytes[0]);
    }

    static std::array<uint64_t, 2> static_mask;
    std::array<uint64_t, 2> mFields;
};


#endif // MASKFILTER_H
