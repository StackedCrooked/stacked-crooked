#ifndef NATIVEFILTER_H
#define NATIVEFILTER_H


#include "Networking.h"
#include <cstdint>


struct NativeFilter
{
    NativeFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    bool match(const uint8_t* packet_data, uint32_t /*len*/) const
    {
        const auto& ip_header = *reinterpret_cast<const IPv4Header*>(packet_data + sizeof(EthernetHeader));
        const auto& tcp_header = *reinterpret_cast<const TCPHeader*>(packet_data + sizeof(EthernetHeader) + sizeof(IPv4Header));

        return (ip_header.mProtocol == mProtocol)
            && (ip_header.mSourceIP == mSourceIP)
            && (ip_header.mDestinationIP == mDestinationIP)
            && (tcp_header.mSourcePort == mSourcePort)
            && (tcp_header.mDestinationPort == mDestinationPort);
    }

private:
    uint8_t mProtocol;
    IPv4Address mSourceIP;
    IPv4Address mDestinationIP;
    Net16 mSourcePort;
    Net16 mDestinationPort;
};


#endif // NATIVEFILTER_H
