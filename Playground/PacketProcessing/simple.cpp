#ifndef PREFETCH
#error "PREFETCH is not defined."
#endif


#ifndef FILTERTYPE
#error "FILTERTYPE is not defined."
#endif

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
struct FlowImpl
{
    FlowImpl(uint8_t protocol, IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port) :
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


using Flow = FlowImpl<FILTERTYPE>;


enum : uint64_t { flow_mod = 32 };

struct Flows
{
    Flows()
    {
        mFlows.reserve(512);
        for (std::vector<uint32_t>& vec : mBuckets)
        {
            vec.reserve(512/flow_mod);
        }
    }


    void report_matches(std::vector<uint64_t>& matches)
    {
        for (auto i = 0ul; i != mFlows.size(); ++i)
        {
            std::cout << "flow_index=" << i << " bucket=" << find_bucket_index(mFlows[i]) << " matches=" << matches[i] << std::endl;
        }

        std::cout << std::endl;
        for (std::vector<uint32_t>& bucket : mBuckets)
        {
            if (!bucket.empty())
            {
                std::cout << "\tbucket.size=" << bucket.size() << "\t\t";
                auto count = 20ul;
                for (auto& i : bucket)
                {
                    std::cout << " " << i;
                    if (!count--)
                    {
                        std::cout << "...";
                        break;
                    }
                }
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    }

    uint32_t find_bucket_index(Flow& flow)
    {
        auto flow_index = &flow - mFlows.data();
        for (std::vector<uint32_t>& vec : mBuckets)
        {
            auto it = std::find(vec.begin(), vec.end(), flow_index);
            if (it != vec.end())
            {
                return &vec - mBuckets.data();
            }
        }
        return -1;
    }


    bool empty()
    {
        return mBuckets.empty();
    }

    void add_flow(uint8_t protocol, IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port)
    {
        auto flow_index = mFlows.size();
        auto& flow_vec = mBuckets[dst_port % flow_mod];
        mFlows.emplace_back(protocol, source_ip, target_ip, src_port, dst_port);
        flow_vec.push_back(flow_index);
    }

    std::size_t size() const
    {
        return mFlows.size();
    }

    uint32_t match(const Packet& packet, uint64_t* matches)
    {
        auto tcp_header = Decode<TCPHeader>(packet.data() + sizeof(EthernetHeader) + sizeof(IPv4Header));

        uint32_t total_matches = 0;

        std::vector<uint32_t>& vec = mBuckets[tcp_header.mDestinationPort.hostValue() % flow_mod];
        for (uint32_t flow_index : vec)
        {
            Flow& flow = mFlows[flow_index];
            if (flow.match(packet.data(), packet.size()))
            {
                matches[flow_index]++;
                total_matches++;
                break;
            }
        }

        return total_matches;
    }

    std::array<std::vector<uint32_t>, flow_mod> mBuckets;
    std::vector<Flow> mFlows;
};


template<typename FilterType, uint32_t prefetch>
void test(const std::vector<Packet>& packets, Flows& flows, uint64_t* const matches)
{
    const uint32_t num_flows = flows.size();
    const auto start_time = Clock::now();
    const auto num_packets = packets.size();

    auto packets_ptr = packets.data();
    uint32_t total_matches = 0;

    for (auto& packet : packets)
    {
        if (prefetch)
        {
            __builtin_prefetch((packets_ptr + prefetch)->data() + sizeof(EthernetHeader) + sizeof(IPv4Header), 0, 0);
        }

        total_matches += flows.match(packet, matches);
    }

    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start_time).count();
    auto ns_per_packet = 1.0 * elapsed_ns / num_packets;
    auto mpps = 1e9 / ns_per_packet / 1000000;
    auto mpps_rounded = int(0.5 + 100 * mpps)/100.0;

    std::cout << std::setw(12) << std::left << GetTypeName<FilterType>()
            << " PREFETCH=" << prefetch
            << " FLOWS=" << std::setw(4) << std::left << num_flows
            << " total_matches=" << total_matches
            //<< " ns_per_packet=" << ns_per_packet
            << " MPPS=" << std::setw(9) << std::left << mpps_rounded
            << " (" << (num_flows * mpps_rounded) / flow_mod << " million filter comparisons per second)"
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


struct Tuple
{
    IPv4Address src_ip = IPv4Address(1, 1, 1, 1);
    IPv4Address dst_ip = IPv4Address(1, 1, 1, 1);
    uint16_t src_port = 1;
    uint16_t dst_port = 1;

    std::tuple<IPv4Address, IPv4Address, uint16_t, uint16_t> tie() const
    {
        return std::make_tuple(src_ip, dst_ip, src_port, dst_port);
    }

    friend bool operator<(const Tuple& lhs, const Tuple& rhs)
    {
        return lhs.tie() < rhs.tie();
    }

    friend bool operator==(const Tuple& lhs, const Tuple& rhs)
    {
        return lhs.tie() == rhs.tie();
    }
};



template<typename FilterType, int prefetch>
void do_run(uint32_t num_packets, uint32_t num_flows)
{
    std::vector<Packet> packets;
    packets.reserve(num_packets);

    Flows flows;

    std::vector<uint16_t> dst_ports;
    dst_ports.reserve(num_flows);
    for (auto i = 0ul; i != num_flows; ++i)
    {
        dst_ports.push_back(1 + i % flow_mod);
    }
    std::random_shuffle(begin(dst_ports), end(dst_ports));


    std::vector<Tuple> tuples;
    tuples.resize(num_flows);

    for (auto i = 0ul; i != num_flows; ++i)
    {
        Tuple& t = tuples[i];
        t.dst_ip.mData[3] = i;//num_flows / dst_ports[i];
        t.dst_port = dst_ports[i];
    }

    for (auto i = 1ul; i <= num_packets; ++i)
    {
        auto& tup = tuples[i % num_flows];
        packets.emplace_back(6, tup.src_ip, tup.dst_ip, tup.src_port, tup.dst_port);
    }

    //std::cout << "==== tuples.size=" << tuples.size() << " num_flows=" << num_flows << std::endl;

    for (auto i = 1ul; i <= num_flows; ++i)
    {
        const Tuple& tup = tuples[i - 1];
        flows.add_flow(6, tup.src_ip, tup.dst_ip, tup.src_port, tup.dst_port);
    }

    std::vector<uint64_t> matches(num_flows);
    test<FilterType, prefetch>(packets, flows, matches.data());
    //flows.report_matches(matches);
    std::cout << std::endl;

}


template<typename FilterType>
void run(uint32_t num_packets = 1000 * 1000)
{
    int flow_counts[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };

    for (auto flow_count : flow_counts)
    {
        do_run<FilterType, PREFETCH>(num_packets, flow_count);
    }
    std::cout << std::endl;
}


int main()
{
    srand(time(0));
    run<FILTERTYPE>();
    std::cout << std::endl;
}
