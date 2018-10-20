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

        return (mFields[0] == (mMask[0] & u64_data[0]))
            && (mFields[1] == (mMask[1] & u64_data[1]));
    }

private:
    static std::array<uint64_t, 2> GetMask();

    std::array<uint64_t, 2> mMask;
    std::array<uint64_t, 2> mFields;
};
