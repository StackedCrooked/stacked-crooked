#include "BBPort.h"
#include "BBInterface.h"
#include "BBServer.h"
#include "Clock.h"
#include "Networking.h"
#include "PhysicalInterface.h"
#include <algorithm>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>


#define ASSERT_EQ(x, y) if (x != y) { std::cout << (x) << " != " << (y) << std::endl; assert(false); }


template<typename T>
void append(std::vector<uint8_t>& vec, T value)
{
    auto old_size = vec.size();
    vec.resize(old_size + sizeof(value));
    memcpy(vec.data() + old_size, &value, sizeof(value));
}


MACAddress generate_mac(uint32_t i)
{
    MACAddress mac{{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x00}};
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
    num_packets = 1000 * 1000,
    num_iterations = num_packets / 32
};


static_assert(num_packets % 32 == 0, "");



int64_t run_test(BBServer& bbServer, const std::vector<std::vector<uint8_t>>& vec)
{
    auto start_time = Benchmark::start();

    //ASSERT_EQ(vec.size(), 32);
    bbServer.run(vec, num_packets);
    auto elapsed_time = Benchmark::stop() - start_time;
    return elapsed_time;
}


BBServer bbServer;


void run(const std::vector<std::vector<uint8_t>>& packets)
{

    std::array<int64_t, 64> tests;

    for (auto& ns : tests)
    {
        ns = run_test(bbServer, packets);
    }

    std::sort(tests.begin(), tests.end());

    auto print_cycles = [](const char* message, int64_t cycles)

    {
        auto cycles_per_packet = 1.0 * cycles / num_packets;
        auto ns_per_packet = 1e9 * cycles / cpu_hz / num_packets;
        auto budget_usage = int(0.5 + 100 * ns_per_packet / 25.0);
        auto Mpps = int(0.5 + 1e3 * num_packets / cycles_to_ns(cycles));
        std::cout
            << message
            << " budget_consumed=" << budget_usage << "%"
            << " Mpps=" << Mpps
            << " ns_per_packet=" << int(0.5 + ns_per_packet)
            << " cycles_per_packet=" << int(0.5 + cycles_per_packet)
            << std::endl;
    };

    print_cycles("BEST  : ", tests.front());
    print_cycles("MEDIAN: ", tests[tests.size()/2]);
    print_cycles("WORST : ", tests.back());
    std::cout << "===" << std::endl;
}


// TODO: Enable dynamic config:
// - number of ports
// - number of flows per port
// - etc..


int main()
{

    std::vector<std::vector<uint8_t>> packets;
    packets.reserve(32u * 8);
    for (auto flow_index = 0; flow_index != 32u; ++flow_index)
    {
        for (auto i = 0u; i != 8u; ++i) // bursts of 8
        {
            packets.push_back(make_packet(flow_index + 1));
        }
    }

    for (auto flow_index = 0; flow_index != 32; ++flow_index)
    {
        bbServer.getPhysicalInterface(0).getBBInterface(flow_index).addPort(generate_mac(flow_index + 1)).addUDPFlow(flow_index + 1);
    }

    srand(time(0));

    for (auto i = 0; i != 4; ++i)
    {
        std::random_shuffle(packets.begin(), packets.end());
        run(packets);
    }

    for (auto i = 0; i != 32; ++i)
    {
        assert(bbServer.getPhysicalInterface(0).getBBInterface(i).getBBPort(0).mBroadcastCounter == 0);
        ASSERT_EQ(bbServer.getPhysicalInterface(0).getBBInterface(i).getBBPort(0).mTotalCounter, bbServer.getPhysicalInterface(0).getBBInterface(i).getBBPort(0).mUnicastCounter);
        std::cout << "Flow " << (i + 1) << " Unicast=" << bbServer.getPhysicalInterface(0).getBBInterface(i).getBBPort(0).mUnicastCounter << "/" << bbServer.getPhysicalInterface(0).getBBInterface(i).getBBPort(0).mTotalCounter << std::endl;
    }
}
