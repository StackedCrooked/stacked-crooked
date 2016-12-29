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
        enum { offset = sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(uint16_t) + sizeof(uint16_t) - sizeof(mMasks) };

        using U64 = std::array<uint64_t, 2>;
        auto u64 = Decode<U64>(packet_data + offset);

        return (mFields[1] == (mMasks[1] & u64[1]))
            && (mFields[0] == (mMasks[0] & u64[0]));
    }

private:
    std::array<uint64_t, 2> mFields;
    std::array<uint64_t, 2> mMasks;
};


#endif // MASKFILTER_H
