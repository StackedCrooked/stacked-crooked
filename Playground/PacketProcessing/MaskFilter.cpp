#include "MaskFilter.h"


MaskFilter::MaskFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
{
    // Composite of IPv4 + TCP/UDP header fields from TTL to DestinationPort.
    struct TransportHeader
    {
        uint8_t ttl;
        uint8_t protocol;
        uint16_t checksum;
        IPv4Address src_ip = IPv4Address();
        IPv4Address dst_ip = IPv4Address();
        Net16 src_port;
        Net16 dst_port;
    };

    auto h = TransportHeader();
    h.protocol = protocol;
    h.src_ip = src_ip;
    h.dst_ip = dst_ip;
    h.src_port = Net16(src_port);
    h.dst_port = Net16(dst_port);

    static_assert(sizeof(mFields) == sizeof(h), "");

    memcpy(&mFields[0], &h, sizeof(mFields));

}


std::array<uint64_t, 2> MaskFilter::static_mask = MaskFilter::GetMask();