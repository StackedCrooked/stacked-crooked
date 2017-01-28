#ifndef BPFFILTER_H
#define BPFFILTER_H


#include "Networking.h"
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include "pcap.h"


struct BPFFilter
{

    BPFFilter(std::string match_bpf);

    BPFFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    bool match(const uint8_t* data, uint32_t size) const
    {
        auto ip_data = data + sizeof(EthernetHeader);
        auto ip_size = size - sizeof(EthernetHeader);
        return match_bpf(mProgram.bf_insns, ip_data, ip_size, ip_size) != 0;
    }

private:
    static std::string generate_bpf_filter_string(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    #define BPFFILTER_EXTRACT_SHORT(p)  ((u_short)ntohs(*(u_short *)p))
    #define BPFFILTER_EXTRACT_LONG(p) (ntohl(*(uint32_t *)p))

    static u_int match_bpf(const struct bpf_insn* pc, const u_char* p, u_int wirelen, u_int buflen);

    bpf_program mProgram;
};


#endif // BPFFILTER_H
