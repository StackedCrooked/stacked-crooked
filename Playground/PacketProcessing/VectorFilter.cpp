#include "VectorFilter.h"


VectorFilter::VectorFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
{
    // Composite of IPv4 + TCP/UDP header fields from TTL to DestinationPort.
    struct TransportHeader
    {
        uint8_t ttl;
        uint8_t protocol;
        uint16_t checksum;
        IPv4Address src_ip;
        IPv4Address dst_ip;
        Net16 src_port;
        Net16 dst_port;
    };

    auto h = TransportHeader();
    h.protocol = protocol;
    h.src_ip = src_ip;
    h.dst_ip = dst_ip;
    h.src_port = src_port;
    h.dst_port = dst_port;

    field_ = _mm_loadu_si128((__m128i*)&h);
}


__m128i VectorFilter::static_mask_;
