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
}


static bool test_parsed_filter()
{
    IPv4Address src_ip(1, 1, 1, 1);
    IPv4Address dst_ip(1, 1, 1, 2);
    uint16_t src_port = 1001;
    uint16_t dst_port = 1002;

    auto bpf_string = BPFFilter::generate_bpf_filter_string(ProtocolId::TCP, src_ip, dst_ip, src_port, dst_port);
    Parser parser(bpf_string.c_str());
    Expression e = parser.parse();
    e.print();


    Packet p(ProtocolId::TCP, src_ip, dst_ip, src_port, dst_port);

    BPFFilter bpf_filter(bpf_string);
    assert(bpf_filter.match(p.data(), p.size()));
    assert(e.match(p.data(), p.size(), sizeof(EthernetHeader), sizeof(EthernetHeader) + sizeof(IPv4Header)));

    return true;
}



static bool b = test_parsed_filter();
