#include "ParsedFilter.h"
#include "Packet.h"
#include <cassert>
#include <iostream>


void test_udp_payload(const char* bpf_text, uint32_t payload_value)
{
    try
    {
        Parser p(bpf_text);
        std::cerr << p.mText << std::endl;
        Expression e = p.parse();
        e.print();

        auto packet = Packet(ProtocolId::UDP, IPv4Address(1, 1, 1, 1), IPv4Address(1, 1, 1, 2), 1024, 1024);
        auto l3_offset = sizeof(EthernetHeader);
        auto l4_offset = l3_offset + sizeof(IPv4Header);


        packet.data()[l4_offset + sizeof(UDPHeader)] = payload_value;
        std::cout << "MATCH? => " << e.match(packet.data(), packet.size(), l3_offset, l4_offset) << std::endl;

        packet.data()[l4_offset + sizeof(UDPHeader)] = payload_value + 1;
        std::cout << "MATCH? => " << e.match(packet.data(), packet.size(), l3_offset, l4_offset) << std::endl;

        packet.data()[l4_offset + sizeof(UDPHeader)] = payload_value;
        std::cout << "MATCH? => " << e.match(packet.data(), packet.size(), l3_offset, l4_offset) << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
}


void test_length_or(const char* bpf_text, int length, bool pass)
{
    try
    {
        Parser p(bpf_text);
        std::cerr << p.mText << std::endl;
        Expression e = p.parse();
        e.print();

        auto packet = Packet(ProtocolId::UDP, IPv4Address(1, 1, 1, 1), IPv4Address(1, 1, 1, 2), 1024, 1024);
        auto l3_offset = sizeof(EthernetHeader);
        auto l4_offset = l3_offset + sizeof(IPv4Header);

        std::cout << "len=" << length << " MATCH? => " << (e.match(packet.data(), length, l3_offset, l4_offset)) << std::endl;
        assert(e.match(packet.data(), length, l3_offset, l4_offset) == pass);
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
}



int main()
{
    test_length_or("(ip and udp and len=60) "
                   "|| (ip and udp and len=124) "
                   "|| (ip and udp and len=252) "
                   "|| (ip and udp and len=508) ", 60, true);
    test_length_or("(ip and udp and len=60) "
                   "|| (ip and udp and len=124) "
                   "|| (ip and udp and len=252) "
                   "|| (ip and udp and len=508) ", 252, true);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    test_length_or("(ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=60) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=124) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=252) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=508) ", 60, true);

    test_length_or("(ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=60) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=124) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=252) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=508) ", 124, true);

    test_length_or("(ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=60) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=124) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=252) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=508) ", 252, true);

    test_length_or("(ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=60) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=124) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=252) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=508) ", 508, true);

    test_length_or("(ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=60) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=124) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=252) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=508) ", 100, false);

    test_length_or("(ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=60) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=124) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=252) \n"
                   "|| (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and len=508) ", 64, false);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    test_udp_payload("ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and udp[8:1]=123", 123);
    test_udp_payload("ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and udp[8:1]=0xFE", 0xFE);
    test_udp_payload("ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and udp[8:1]=0xF", 0xF);
    test_udp_payload("ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and udp[8:1]=0x1", 0x1);
    test_udp_payload("ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and udp[8:1]=0xFEFE", 0xFEFE);



    test_udp_payload("(ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and udp[8:1]=0x1) or (ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and udp[8:1]=0x11)", 0x1);
}
