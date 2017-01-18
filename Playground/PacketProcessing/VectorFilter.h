#ifndef VECTORFILTER_H
#define VECTORFILTER_H


#include "Networking.h"
#include <cstdint>
#include <x86intrin.h>


struct VectorFilter
{
    VectorFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    bool match(const uint8_t* packet_data, uint32_t /*len*/) const
    {
        enum { offset = sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(uint16_t) + sizeof(uint16_t) - sizeof(static_mask_) };

        auto mask_result = _mm_and_si128(static_mask_, _mm_loadu_si128((__m128i*)(packet_data + offset)));
        auto compare_result = _mm_xor_si128(field_, mask_result);
        return _mm_testz_si128(compare_result, compare_result);
    }

private:
    static __m128i GetMask()
    {
        uint8_t mask_bytes[16] = {
            0x00, 0xff, 0x00, 0x00, // ttl, protocol and checksum
            0xff, 0xff, 0xff, 0xff, // source ip
            0xff, 0xff, 0xff, 0xff, // destination ip
            0xff, 0xff, 0xff, 0xff  // source and destination ports
        };

        return _mm_loadu_si128((__m128i*)&mask_bytes[0]);
    }

    __m128i field_;
    static __m128i static_mask_;
};


#endif // VECTORFILTER_H
