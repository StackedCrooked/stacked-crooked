#include "Utils.h"
#include "Networking.h"
#include "NativeFilter.h"
#include "Packet.h"
#include "VectorFilter.h"
#include "BPFFilter.h"
#include "MaskFilter.h"
#include "PCAPWriter.h"
#include <iomanip>
#include <iostream>
#include <vector>


template<typename FilterType>
struct Flow
{
    Flow(uint8_t protocol, IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port) :
        mFilter(protocol, source_ip, target_ip, src_port, dst_port)
    {
    }

    bool match(const uint8_t* frame_bytes, int len) const
    {
        // avoid unpredictable branch here.
        return mFilter.match(frame_bytes, len);
    }

private:
    FilterType mFilter;
};

volatile const unsigned volatile_zero = 0;



template<typename FilterType, uint32_t prefetch>
void test(const std::vector<Packet>& packets, std::vector<Flow<FilterType>>& flows, uint64_t* const matches)
{
    const uint32_t num_flows = flows.size();
    const auto start_time = Clock::now();
    const auto num_packets = packets.size();

    for (auto i = 0ul; i != num_packets; ++i)
    {
        const Packet& packet = packets[i];

        if (prefetch > 0)
        {
            auto prefetch_packet = &packet + prefetch;
            auto prefetch_ptr = prefetch_packet->data() + sizeof(EthernetHeader) + sizeof(IPv4Header);
            __builtin_prefetch(prefetch_ptr, 0, 0);
        }

        auto packet_data = packet.data();
        auto packet_size = packet.size();

        for (auto flow_index = 0ul; flow_index != num_flows; ++flow_index)
        {
            matches[flow_index] += flows[flow_index].match(packet_data, packet_size);
        }
    }

    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start_time).count();
    auto ns_per_packet = 1.0 * elapsed_ns / num_packets;
    auto mpps = 1e9 / ns_per_packet / 1000000;
    auto mpps_rounded = int(0.5 + 100 * mpps)/100.0;

    std::cout << std::setw(12) << std::left << GetTypeName<FilterType>()
            << " PREFETCH=" << prefetch
            << " FLOWS=" << std::setw(4) << std::left << num_flows
            << " MPPS=" << std::setw(9) << std::left << mpps_rounded
            << " (" << num_flows * mpps_rounded << " million filter comparisons per second)"
            ;

    #if 1
    std::cout << " (verify-matches:";
    for (auto i = 0ul; i != std::min(num_flows, 10u); ++i)
    {
        if (i > 0) std::cout << ',';
        std::cout << int(10000.0 * matches[i] / packets.size())/100.0 << '%';
    }
    if (num_flows > 20)
    {
        std::cout << "...";
    }
    std::cout << ")";
    #endif

}



template<typename FilterType, int prefetch>
void do_run(uint32_t num_packets, uint32_t num_flows)
{
    std::vector<Packet> packets;
    packets.reserve(num_packets);

    std::vector<Flow<FilterType>> flows;
    flows.reserve(num_flows);

    for (auto i = 1ul; i <= num_packets; ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1);
        IPv4Address dst_ip(1, 1, 1, 1);
        uint16_t src_port = i % num_flows;
        uint16_t dst_port = i % num_flows;
        packets.emplace_back(6, src_ip, dst_ip, src_port, dst_port);
    }

    for (auto i = 1ul; i <= num_flows; ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1);
        IPv4Address dst_ip(1, 1, 1, 1);
        uint16_t src_port = i % num_flows;
        uint16_t dst_port = i % num_flows;
        flows.emplace_back(6, src_ip, dst_ip, src_port, dst_port);
    }

    std::vector<uint64_t> matches(num_flows);
    test<FilterType, prefetch>(packets, flows, matches.data());
    std::cout << std::endl;

}




template<typename FilterType>
void run(uint32_t num_packets = 1024 * 1024)
{
    int flow_counts[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256 };

    for (auto flow_count : flow_counts)
    {
        do_run<FilterType, 0>(num_packets, flow_count);
    }
    std::cout << std::endl;

    for (auto flow_count : flow_counts)
    {
        do_run<FilterType, 4>(num_packets, flow_count);
    }
    std::cout << std::endl;
}


int main()
{
    run<MaskFilter>();
    std::cout << std::endl;

    run<VectorFilter>();
    std::cout << std::endl;

    run<BPFFilter>();
    std::cout << std::endl;
}

