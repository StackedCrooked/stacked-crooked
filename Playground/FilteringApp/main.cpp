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


MACAddress mac{{ 0x00, 0x25, 0x90, 0x31, 0x82, 0x06}};


std::vector<uint8_t> make_packet(uint16_t dst_port)
{
    std::vector<uint8_t> result;
    result.reserve(128);
    append(result, EthernetHeader::Create(mac));
    append(result, IPv4Header::Create(ProtocolId::UDP, IPv4Address::Create(1), IPv4Address::Create(1)));
    append(result, UDPHeader::Create(1, dst_port));
    return result;
}





enum
{
    num_packets = 1000 * 1000,
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
    auto& bbPort = bbServer.mPhysicalInterface.getBBInterface(0).addPort(mac);


    bbPort.addUDPFlow(1);
    bbPort.addUDPFlow(2);
    bbPort.addUDPFlow(3);
    bbPort.addUDPFlow(4);

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
        std::cout << "budget_consumed=" << budget_usage << "% Mpps=" << Mpps << " ns_per_packet=" << int(0.5 + ns_per_packet) << " cycles_per_packet=" << int(0.5 + 3.5 * ns_per_packet) << " UnicastCounter=" << bbPort.mUnicastCounter << " BroadcastCounter=" << bbPort.mBroadcastCounter << std::endl;
        for (const UDPFlow& flow : bbPort.mUDPFlows)
        {
            std::cout << "Flow result: " << flow.mPacketsReceived << std::endl;
        }
        break;
    }

    assert(bbPort.mUnicastCounter == num_packets * tests.size());
    assert(bbPort.mUDPFlows[0].mPacketsReceived == num_packets * tests.size() / 4);
    assert(bbPort.mUDPFlows[1].mPacketsReceived == num_packets * tests.size() / 4);
    assert(bbPort.mUDPFlows[2].mPacketsReceived == num_packets * tests.size() / 4);
    assert(bbPort.mUDPFlows[3].mPacketsReceived == num_packets * tests.size() / 4);
}


int main()
{

    std::vector<std::vector<uint8_t>> packets;
    packets.reserve(32);
    for (auto i = 0u; i != 8u; ++i) { packets.push_back(make_packet(1)); }
    for (auto i = 0u; i != 8u; ++i) { packets.push_back(make_packet(2)); }
    for (auto i = 0u; i != 8u; ++i) { packets.push_back(make_packet(3)); }
    for (auto i = 0u; i != 8u; ++i) { packets.push_back(make_packet(4)); }

    //std::random_shuffle(packets.begin(), packets.end());

    for (auto i = 0; i != 4; ++i)
    {
        run(packets);
    }
}



// Prepare packets
// MakeUDPPacket
