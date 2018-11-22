#include "BPFCompositeExpression.h"
#include <sstream>


BPFCompositeExpression::BPFCompositeExpression()
{
}


void BPFCompositeExpression::add(BPFExpression& expr)
{
    if (auto derived = dynamic_cast<BPFCompositeExpression*>(&expr))
    {
        add_impl(*derived);
    }
    else if (auto derived = dynamic_cast<Length*>(&expr))
    {
        add_impl(*derived);
    }
    else if (auto derived = dynamic_cast<L3Type*>(&expr))
    {
        add_impl(*derived);
    }
    else if (auto derived = dynamic_cast<L4Type*>(&expr))
    {
        add_impl(*derived);
    }
    else if (auto derived = dynamic_cast<SourceIPv4*>(&expr))
    {
        add_impl(*derived);
    }
    else if (auto derived = dynamic_cast<DestinationIPv4*>(&expr))
    {
        add_impl(*derived);
    }
    else if (auto derived = dynamic_cast<SourceOrDestinationIPv4*>(&expr))
    {
        add_impl(*derived);
    }
    else if (auto derived = dynamic_cast<SourcePort*>(&expr))
    {
        add_impl(*derived);
    }
    else if (auto derived = dynamic_cast<DestinationPort*>(&expr))
    {
        add_impl(*derived);
    }
    else if (auto derived = dynamic_cast<SourceOrDestinationPort*>(&expr))
    {
        add_impl(*derived);
    }
}


std::string BPFCompositeExpression::toStringImpl() const
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
