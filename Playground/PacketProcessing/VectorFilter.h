#ifndef VECTORFILTER_H
#define VECTORFILTER_H


#include "Networking.h"
#include <cstdint>
#include <x86intrin.h>


struct VectorFilter
{
    VectorFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
    {
        // Composite of IPv4 + TCP/UDP header fields from TTL to DestinationPort.
        struct TransportHeader
        {
            uint8_t ttl;
            uint8_t protocol;
            uint16_t checksum;
            IPv4Address src_ip = IPv4Address();
            IPv4Address dst_ip = IPv4Address();
            uint16_t src_port = 0;
            uint16_t dst_port = 0;
        };

        auto h = TransportHeader();
        h.protocol = protocol;
        h.src_ip = src_ip;
        h.dst_ip = dst_ip;
        h.src_port = src_port;
        h.dst_port = dst_port;

        field_ = _mm_loadu_si128((__m128i*)&h);

        uint8_t mask_bytes[16] = {
            0x00, 0xff, 0x00, 0x00, // ttl, protocol and checksum
            0xff, 0xff, 0xff, 0xff, // source ip
            0xff, 0xff, 0xff, 0xff, // destination ip
            0xff, 0xff, 0xff, 0xff  // source and destination ports
        };
        mask_ = _mm_loadu_si128((__m128i*)&mask_bytes[0]);
    }

    bool match(const uint8_t* packet_data, uint32_t /*len*/) const
    {
        enum { offset = sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(uint16_t) + sizeof(uint16_t) - sizeof(mask_) };

        __m128i mask_result = _mm_cmpeq_epi32(
            field_,
            _mm_and_si128(
                mask_,
                _mm_loadu_si128((__m128i*)(packet_data + offset)))
        );

        __m128i compare_result = _mm_cmpeq_epi8(mask_result, _mm_setzero_si128());
        return _mm_testz_si128(compare_result, compare_result);
    }

    __m128i field_;
    __m128i mask_;
};


#endif // VECTORFILTER_H
