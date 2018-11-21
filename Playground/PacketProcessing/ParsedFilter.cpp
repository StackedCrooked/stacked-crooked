#include "ParsedFilter.h"
#include "Packet.h"


ParsedFilter::ParsedFilter(const std::string& bpf) :
    mExpression(Expression::True())
{
    Parser p(bpf.c_str());
    mExpression = p.parse();
}

ParsedFilter::ParsedFilter(ProtocolId protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
    ParsedFilter(BPFFilter::generate_bpf_filter_string(protocol, src_ip, dst_ip, src_port, dst_port))
{
    assert(dst_port != 0);
}


