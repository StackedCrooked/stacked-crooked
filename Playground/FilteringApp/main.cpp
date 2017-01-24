#include "BBPort.h"
#include "BBInterface.h"
#include "BBServer.h"
#include "Networking.h"
#include "PhysicalInterface.h"
#include <algorithm>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>



template<typename T>
void append(std::vector<uint8_t>& vec, T value)
{
    auto old_size = vec.size();
    vec.resize(old_size + sizeof(value));
    memcpy(vec.data() + old_size, &value, sizeof(value));
}


MACAddress generate_mac(uint32_t i)
{
    MACAddress mac{{ 0x00, 0x25, 0x90, 0x31, 0x82, 0x06}};
    mac.data()[5] = i;
    return mac;
}




std::vector<uint8_t> make_packet(uint16_t dst_port)
{
    std::vector<uint8_t> result;
    result.reserve(128);
    append(result, EthernetHeader::Create(generate_mac(dst_port)));
    append(result, IPv4Header::Create(ProtocolId::UDP, IPv4Address::Create(1), IPv4Address::Create(1)));
    append(result, UDPHeader::Create(1, dst_port));
    return result;
}





enum
{
    num_packets = 2 * 1000 * 1000,
    num_iterations = num_packets / 32
};


static_assert(num_packets % 32 == 0, "");



using SteadyClock = std::chrono::steady_clock;


std::chrono::nanoseconds run_test(BBServer& bbServer, const std::vector<std::vector<uint8_t>>& vec)
{
    auto start_time = SteadyClock::now();

    assert(vec.size() == 32);
    bbServer.run(vec, num_packets);
    auto elapsed_time = SteadyClock::now() - start_time;
    return elapsed_time;
}


void run(const std::vector<std::vector<uint8_t>>& packets)
{

    BBServer bbServer;
    bbServer.getPhysicalInterface(0).getBBInterface(0).addPort(generate_mac(1)).addUDPFlow(1);
    bbServer.getPhysicalInterface(0).getBBInterface(1).addPort(generate_mac(2)).addUDPFlow(2);
    bbServer.getPhysicalInterface(0).getBBInterface(2).addPort(generate_mac(3)).addUDPFlow(3);
    bbServer.getPhysicalInterface(0).getBBInterface(3).addPort(generate_mac(4)).addUDPFlow(4);

    std::array<std::chrono::nanoseconds, 64> tests;

    for (auto& ns : tests)
    {
        ns = run_test(bbServer, packets);
    }

    std::sort(tests.begin(), tests.end());

    for (auto& ns : tests)
    {
        auto ns_per_packet = 1.0 * ns.count() / num_packets;
        auto budget_usage = int(0.5 + 100 * ns_per_packet / 25.0);
        auto Mpps = int(1.0 * num_packets / (ns.count() / 1e3));
        std::cout << "budget_consumed=" << budget_usage << "% Mpps=" << Mpps << " ns_per_packet=" << int(0.5 + ns_per_packet) << " cycles_per_packet=" << int(0.5 + 3.5 * ns_per_packet) << std::endl;

        std::cout << bbServer.getPhysicalInterface(0).getBBInterface(0).getBBPort(0).getUDPFlow(0).mPacketsReceived << std::endl;
        std::cout << bbServer.getPhysicalInterface(0).getBBInterface(1).getBBPort(0).getUDPFlow(0).mPacketsReceived << std::endl;
        std::cout << bbServer.getPhysicalInterface(0).getBBInterface(2).getBBPort(0).getUDPFlow(0).mPacketsReceived << std::endl;
        std::cout << bbServer.getPhysicalInterface(0).getBBInterface(3).getBBPort(0).getUDPFlow(0).mPacketsReceived << std::endl;
        break;
    }

    assert(bbServer.getPhysicalInterface(0).getBBInterface(0).getBBPort(0).getUDPFlow(0).mPacketsReceived == num_packets * tests.size() / 4);
    assert(bbServer.getPhysicalInterface(0).getBBInterface(1).getBBPort(0).getUDPFlow(0).mPacketsReceived == num_packets * tests.size() / 4);
    assert(bbServer.getPhysicalInterface(0).getBBInterface(2).getBBPort(0).getUDPFlow(0).mPacketsReceived == num_packets * tests.size() / 4);
    assert(bbServer.getPhysicalInterface(0).getBBInterface(3).getBBPort(0).getUDPFlow(0).mPacketsReceived == num_packets * tests.size() / 4);
}


// TODO: Enable dynamic config:
// - number of ports
// - number of flows per port
// - etc..


int main()
{

    std::vector<std::vector<uint8_t>> packets;
    packets.reserve(32);
    for (auto i = 0u; i != 8u; ++i) { packets.push_back(make_packet(1)); }
    for (auto i = 0u; i != 8u; ++i) { packets.push_back(make_packet(2)); }
    for (auto i = 0u; i != 8u; ++i) { packets.push_back(make_packet(3)); }
    for (auto i = 0u; i != 8u; ++i) { packets.push_back(make_packet(4)); }

    std::srand(std::time(0));
    //std::random_shuffle(packets.begin() + 8, packets.begin() + 24);
    //std::random_shuffle(packets.begin(), packets.end());

    for (auto i = 0; i != 4; ++i)
    {
        run(packets);
    }
}
