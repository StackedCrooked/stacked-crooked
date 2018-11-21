#ifndef PARSERFILTER_H
#define PARSERFILTER_H


#include "BPFFilter.h"
#include "Networking.h"
#include "Parser.h"
#include "Parser.h"
#include "Expression.h"


struct ParsedFilter
{
    ParsedFilter(const std::string& bpf);
    ParsedFilter(ProtocolId protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    bool match(const uint8_t* packet_data, uint32_t len) const
    {

        // TODO: FR: Caller must provide these values.
        auto l3_offset = sizeof(EthernetHeader);
        auto l4_offset = sizeof(IPv4Header);


        return mExpression.match(packet_data, len, l3_offset, l4_offset);
    }

    Expression mExpression;
};


#endif // PARSERFILTER_H
