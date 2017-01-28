#include "BBPort.h"
#include "BBInterface.h"
#include "BBServer.h"
#include "Clock.h"
#include "Networking.h"
#include "PhysicalInterface.h"
#include <algorithm>
#include <memory>
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
    result.reserve(64);
    append(result, EthernetHeader::Create(generate_mac(dst_port)));
    append(result, IPv4Header::Create(ProtocolId::UDP, IPv4Address::Create(1), IPv4Address::Create(1)));
    append(result, UDPHeader::Create(1, dst_port));
    return result;
}





enum : uint64_t
{
    num_flows = 40,
    num_packets = 4 * 1000UL * 1000UL,
    num_iterations = num_packets / num_flows
};


static_assert(num_packets % num_flows == 0, "");



int64_t run_test(BBServer& bbServer, const std::vector<RxPacket>& rxPackets)
{
    auto start_time = Benchmark::start();
    bbServer.run(rxPackets, num_packets / rxPackets.size());
    auto elapsed_time = Benchmark::stop() - start_time;
    return elapsed_time;
}


void run(BBServer& bbServer, const std::vector<RxPacket>& rxPackets)
{
    std::array<int64_t, 64> tests;

    for (auto& ns : tests)
    {
        ns = run_test(bbServer, rxPackets);
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

    print_cycles("BEST   : ", tests.front());
    print_cycles("MEDIAN : ", tests[tests.size()/2]);
    print_cycles("WORST  : ", tests.back());
    std::cout << "===" << std::endl;
}


int main()
{
#define PRINT_SIZE(x) std::cout << "sizeof(" << #x << ")=" << sizeof(x) << std::endl;
    PRINT_SIZE(PhysicalInterface);
    PRINT_SIZE(BBInterface);
    PRINT_SIZE(RxTrigger);
    PRINT_SIZE(BBPort);
    PRINT_SIZE(UDPFlow);
    PRINT_SIZE(RxPacket);
    auto bbServerPtr = std::make_unique<BBServer>();
    BBServer& bbServer = *bbServerPtr;

    // Create UDP flows
    for (auto flow_index = 0; flow_index != num_flows; ++flow_index)
    {
        bbServer.getPhysicalInterface(0).getBBInterface(flow_index).addPort(generate_mac(flow_index + 1)).addUDPFlow(flow_index + 1);
    }

    std::cout << "num_packets=" << num_packets << std::endl;


    // Create packet buffers and fill them with UDP data
    std::vector<std::vector<uint8_t>> packet_buffers;
    packet_buffers.reserve(num_flows * 8);

    for (auto flow_index = 0; flow_index != num_flows; ++flow_index)
    {
        for (auto i = 0u; i != 8u; ++i) // bursts of 8
        {
            packet_buffers.push_back(make_packet(flow_index + 1));
        }
    }

    // Shuffling makes it harder to efficiently demultiplex packet batches.
    // However, it does not seem to affect speed of per-packet demultiplexing.
    srand(time(0));
    std::random_shuffle(packet_buffers.begin(), packet_buffers.end());

    // Convert to list of RxPacket objects.
    std::vector<RxPacket> rxPackets;
    rxPackets.resize(packet_buffers.size());
    for (RxPacket& rxPacket : rxPackets)
    {
        auto i = &rxPacket - rxPackets.data();
        rxPacket = RxPacket(packet_buffers[i].data(), packet_buffers[i].size(), packet_buffers[i][5] - 1);
    }

    // Run the benchmark a couple of times.
    for (auto i = 0; i != 4; ++i)
    {
        run(bbServer, rxPackets);
    }

    // Verify the counters.
    for (auto i = 0; i != num_flows; ++i)
    {
        BBPort& bbPort = bbServer.getPhysicalInterface(0).getBBInterface(i).getBBPort(0);

        std::cout << "Flow " << (i + 1) << " Accepted=" << bbPort.mUDPAccepted << "/" << bbPort.getTotalCount() << std::endl;
    }
}
