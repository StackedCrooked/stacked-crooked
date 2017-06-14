#include "MaskFilter.h"
#include "MACAddress.h"


MaskFilter::MaskFilter(ProtocolId protocolId, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
    mMask(GetMask())
{
    // Composite of IPv4 + TCP/UDP header fields from TTL to DestinationPort.
    struct MaskHeader
    {
        uint8_t ttl;
        ProtocolId protocol;
        uint16_t checksum;
        IPv4Address src_ip = IPv4Address();
        IPv4Address dst_ip = IPv4Address();
        Net16 src_port;
        Net16 dst_port;
    };

    auto h = MaskHeader();
    h.protocol = protocolId;
    h.src_ip = src_ip;
    h.dst_ip = dst_ip;
    h.src_port = Net16(src_port);
    h.dst_port = Net16(dst_port);

    static_assert(sizeof(mFields) == sizeof(h), "");

    memcpy(&mFields[0], &h, sizeof(mFields));
}


std::array<uint64_t, 2> MaskFilter::GetMask()
{
    static const uint8_t mask_bytes[16] = {
        0x00, 0xff, 0x00, 0x00, // ttl, protocol and checksum
        0xff, 0xff, 0xff, 0xff, // source ip
        0xff, 0xff, 0xff, 0xff, // destination ip
        0xff, 0xff, 0xff, 0xff  // source and destination ports
    };

    return Decode<std::array<uint64_t, 2>>(&mask_bytes[0]);
}
