#include "ParsedFilter.h"
#include "Packet.h"
#include <iostream>


int main()
{
    try
    {
        Parser p("udp[1:2]=0x3 && ip src 1.1.1.1 and ip dst 1.1.1.2 and udp src port 1024 and udp dst port 1024 and udp[8:1]=0x1");
        std::cerr << p.mText << std::endl;
        Expression e = p.parse();
        e.print();

        auto packet = Packet(ProtocolId::UDP, IPv4Address(1, 1, 1, 1), IPv4Address(1, 1, 1, 2), 1024, 1024);
        auto l3_offset = sizeof(EthernetHeader);
        auto l4_offset = l3_offset + sizeof(IPv4Header);


        packet.data()[l4_offset + sizeof(UDPHeader)] = 0x1;
        std::cout << "MATCH? => " << e.match(packet.data(), packet.size(), l3_offset, l4_offset) << std::endl;

        packet.data()[l4_offset + sizeof(UDPHeader)] = 0x2;
        std::cout << "MATCH? => " << e.match(packet.data(), packet.size(), l3_offset, l4_offset) << std::endl;



        packet.data()[l4_offset + sizeof(UDPHeader)] = 0x1;
        std::cout << "MATCH? => " << e.match(packet.data(), packet.size(), l3_offset, l4_offset) << std::endl;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
