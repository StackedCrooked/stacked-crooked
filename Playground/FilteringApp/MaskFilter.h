#pragma once


#include "Networking.h"
#include <array>
#include <cstdint>


struct MaskFilter
{
    MaskFilter(ProtocolId protocolId, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    bool match(const uint8_t* ipv4_header) const
    {
        auto u64_data = Decode<std::array<uint64_t, 2>>(ipv4_header + offsetof(IPv4Header, mTTL));

        return (mFields[0] == (static_mask[0] & u64_data[0]))
            && (mFields[1] == (static_mask[1] & u64_data[1]));
    }

private:
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
