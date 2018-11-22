#include "BPFCompositeExpression.h"
#include <sstream>


BPFCompositeExpression::BPFCompositeExpression()
{
}


std::string BPFCompositeExpression::toString() const
{
    std::stringstream ss;

    int count = 0;

    if (hasFlag(FilterFlags_L3Type )) { ss << (count == 0 ? "" : " and ") << ((mPacketFlags & PacketFlags_IPv4) ? "ip" : "ip6") ; count++ ; }
    if (hasFlag(FilterFlags_SrcIPv4)) { ss << (count == 0 ? "" : " and ") << "ip src " << mSourceIP.toString()                  ; count++ ; }
    if (hasFlag(FilterFlags_DstIPv4)) { ss << (count == 0 ? "" : " and ") << "ip dst " << mSourceIP.toString()                  ; count++ ; }
    if (hasFlag(FilterFlags_L4Type )) { ss << (count == 0 ? "" : " and ") << ((mPacketFlags & PacketFlags_UDP) ? "udp" : "tcp") ; count++ ; }
    if (hasFlag(FilterFlags_SrcPort)) { ss << (count == 0 ? "" : " and ") << "src port " << mSourcePort                         ; count++ ; }
    if (hasFlag(FilterFlags_DstPort)) { ss << (count == 0 ? "" : " and ") << "dst port " << mSourcePort                         ; count++ ; }
    if (hasFlag(FilterFlags_Length )) { ss << (count == 0 ? "" : " and ") << "len=" << mLength                                  ; count++ ; }

    return ss.str();
}
