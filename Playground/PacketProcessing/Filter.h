#pragma once


#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))


#include <cassert>
#include <cstdint>
#include <vector>


struct UDPv4_Tuple_Filter
{
    // IPv4 Header 4 + 4
    // UDP4 Header 2 + 2
    // sa(4)
    // da(4)
    // sp(2) + sp(2)
    // protocol(1)
    std::array<uint64_t, 4> mFields;
    std::array<uint64_t, 4> mMasks;
    std::array<uint8_t , 4> mOffsets;

    bool match(const uint8_t *bytes, int len, MetaData metadata) const
    {
        auto ip_bytes = bytes + sizeof(EthernetHeader);


        auto ip4_header = get_ip4_header(bytes, len, metadata);
        auto udp_header = get_udp_header(bytes, len, metadata);

        bool ok = true;
        ok &= ((bytes + mOffsets[0]) & mMasks[0]) == mFields[0];
        ok &= ((bytes + mOffsets[1]) & mMasks[1]) == mFields[1];
        ok &= ((bytes + mOffsets[2]) & mMasks[2]) == mFields[2];
        ok &= ((bytes + mOffsets[3]) & mMasks[3]) == mFields[3];
        return ok;
    }
};

