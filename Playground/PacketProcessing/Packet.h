#ifndef PACKET_H
#define PACKET_H


#include "Networking.h"
#include <cstdint>


struct Packet
{
    Packet(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
        mPayload()
    {
        auto eth_header = EthernetHeader::Create();
        memcpy(mPayload.data(), &eth_header, sizeof(eth_header));

        auto ip4_ptr = mPayload.data() + sizeof(EthernetHeader);
        auto ip4_header = IPv4Header::Create(protocol, src_ip, dst_ip);
        memcpy(ip4_ptr, &ip4_header, sizeof(ip4_header));

        auto tcp_ptr = ip4_ptr + sizeof(IPv4Header);
        auto tcp_header = TCPHeader::Create(src_port, dst_port);
        memcpy(tcp_ptr, &tcp_header, sizeof(tcp_header));

        #if 0
        {
            PCAPWriter pcap("test.pcap");
            pcap.push_back(mPayload.data(), mPayload.size());
        }
        std::abort();
        #endif
    }

    const uint8_t* data() const { return mPayload.data(); }
    uint32_t size() const { return mPayload.size(); }

private:
    std::array<uint8_t, 64> mPayload;
};


#endif // PACKET_H
