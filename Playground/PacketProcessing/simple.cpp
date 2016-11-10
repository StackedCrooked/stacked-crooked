#include "Utils.h"
#include "Networking.h"
#include "BPFFilter.h"
#include "MaskFilter.h"
#include <iomanip>
#include <iostream>
#include <vector>


struct CombinedHeader
{
    CombinedHeader() = default;

    CombinedHeader(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
    {
        mIPv4Header.mProtocol = protocol;
        mIPv4Header.mSourceIP = src_ip;
        mIPv4Header.mDestinationIP = dst_ip;
        mNetworkHeader.mSourcePort = src_port;
        mNetworkHeader.mDestinationPort = dst_port;
        static_assert(sizeof(*this) == sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(TCPHeader), "");
    }

    uint8_t* data() { return static_cast<uint8_t*>(static_cast<void*>(this)); }
    uint8_t* begin() { return data(); }
    uint8_t* end() { return data() + sizeof(*this); }

    const uint8_t* data() const { return static_cast<const uint8_t*>(static_cast<const void*>(this)); }
    const uint8_t* begin() const { return data(); }
    const uint8_t* end() const { return data() + sizeof(*this); }

    std::size_t size() const { return sizeof(*this); }

    EthernetHeader mEthernetHeader = EthernetHeader();
    IPv4Header mIPv4Header = IPv4Header();
    TCPHeader mNetworkHeader = TCPHeader();
};


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



// Packet distance is 1536 (or 512 * 3) bytes.
// This seems to be the default used by pfring in it's 
// internal Rx buffer.
struct Packet
{
    Packet(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
        mPayload()
    {
        auto ip4_ptr = mPayload.data() + sizeof(EthernetHeader);
        auto ip4_header = IPv4Header::Create(protocol, src_ip, dst_ip);
        memcpy(ip4_ptr, &ip4_header, sizeof(ip4_header));

        auto tcp_ptr = ip4_ptr + sizeof(IPv4Header);
        auto tcp_header = TCPHeader::Create(src_port, dst_port);
        memcpy(tcp_ptr, &tcp_header, sizeof(tcp_header));
	}

    const uint8_t* data() const { return mPayload.data(); }
    uint32_t size() const { return mPayload.size(); }

private:
    std::array<uint8_t, 3 * 512> mPayload;
};


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
            auto prefetch_ptr = prefetch_packet->data() + sizeof(EthernetHeader) + sizeof(IPv4Header) - offsetof(IPv4Header, mTTL);
            __builtin_prefetch(prefetch_ptr, 0, 0);
        }

        auto packet_data = packet.data();
        auto packet_size = packet.size();
        auto flow_index = 0ul;

        while (flow_index + 2 <= num_flows)
        {
            matches[flow_index + 0] += flows[flow_index + 0].match(packet_data, packet_size);
            matches[flow_index + 1] += flows[flow_index + 1].match(packet_data, packet_size);
            flow_index += 2;
        }

        if (flow_index + 1 == num_flows)
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

    for (auto i = 0ul; i < num_flows; ++i)
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
    int flow_counts[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };

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

    for (auto flow_count : flow_counts)
    {
        do_run<FilterType, 8>(num_packets, flow_count);
    }
    std::cout << std::endl;
}


int main()
{
    run<MaskFilter>();
    std::cout << std::endl;

    run<BPFFilter>();
    std::cout << std::endl;

//    run<NativeFilter>();
//    std::cout << std::endl;

//    run<VectorFilter>();
//    std::cout << std::endl;
}

