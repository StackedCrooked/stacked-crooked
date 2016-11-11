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

    uint8_t mask_bytes[16] = {
    0x00, 0xff, 0x00, 0x00, // ttl, protocol and checksum
    0xff, 0xff, 0xff, 0xff, // source ip
    0xff, 0xff, 0xff, 0xff, // destination ip
    0xff, 0xff, 0xff, 0xff  // source and destination ports
    };

    static_assert(sizeof(mask_bytes) == sizeof(mMasks), "");

    memcpy(&mMasks[0], &mask_bytes[0], sizeof(mMasks));
}
