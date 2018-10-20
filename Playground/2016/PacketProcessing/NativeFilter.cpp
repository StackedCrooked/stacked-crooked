#include "NativeFilter.h"


NativeFilter::NativeFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
    mProtocol(protocol),
    mSourceIP(src_ip),
    mDestinationIP(dst_ip),
    mSourcePort(src_port),
    mDestinationPort(dst_port)
{
}
