#include "VectorFilter.h"
#include "Networking.h"


VectorFilter::VectorFilter(ProtocolId protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
{
    // Composite of IPv4 + TCP/UDP header fields from TTL to DestinationPort.
    struct TransportHeader
    {
        uint8_t ttl;
        ProtocolId protocol;
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
    h.src_port = Net16(src_port);
    h.dst_port = Net16(dst_port);


    static_assert(sizeof(field_) == sizeof(h), "");

    field_.load(&h);
}


Vec4ui VectorFilter::static_mask_ = VectorFilter::GetMask();
