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
    static std::string get_bpffilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    BPFFilter(std::string bpf_filter);

    BPFFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
        BPFFilter(get_bpffilter(protocol, src_ip, dst_ip, src_port, dst_port))
    {
    }

    bool match(const uint8_t* data, uint32_t length) const
    {
        return bpf_filter_embedded(mProgram.bf_insns, const_cast<uint8_t*>(data), length, length);
    }

private:
    #define BPFFILTER_EXTRACT_SHORT(p)  ((u_short)ntohs(*(u_short *)p))
    #define BPFFILTER_EXTRACT_LONG(p) (ntohl(*(uint32_t *)p))

    static u_int bpf_filter_embedded(const struct bpf_insn* pc, const u_char* p, u_int wirelen, u_int buflen);

    bpf_program mProgram;
};


#endif // BPFFILTER_H
