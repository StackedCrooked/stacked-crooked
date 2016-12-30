#include "BPFFilter.h"
#include <sstream>
#include <cstdint>


std::string BPFFilter::get_bpffilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
{
    const char* protocol_str = (protocol == 6) ? "tcp" : "(unknown)";

    std::stringstream ss;
    ss  << "ip src " << src_ip
        << " && ip dst " << dst_ip
        << " && " << protocol_str << " src port " << src_port
        << " && " << protocol_str << " dst port " << dst_port
    ;
    return ss.str();
}


BPFFilter::BPFFilter(std::string bpf_filter)
{
    using DummyInterface = std::unique_ptr<pcap_t, decltype(&pcap_close)>;

    DummyInterface dummy_interface(pcap_open_dead(DLT_EN10MB, 1518), &pcap_close);

    if (!dummy_interface)
    {
        throw std::runtime_error("Failed to open pcap dummy interface");
    }

    auto result = pcap_compile(dummy_interface.get(), &mProgram, bpf_filter.c_str(), 1, 0xff000000);
    if (result != 0)
    {
        //std::cout << "pcap_geterr: [" << pcap_geterr(dummy_interface.get()) << "]" << std::endl;
        throw std::runtime_error("pcap_compile failed. Filter=" + bpf_filter);
    }
}
