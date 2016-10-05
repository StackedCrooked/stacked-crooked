#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <x86intrin.h>


struct MAC : std::array<uint8_t, 6>
{
    MAC()
    {
        std::array<uint8_t, 6>& self = *this;
        self = {{ 0x00, 0xff, 0x23, 0x00, 0x00, 0x01 }};
    }
};


struct IPv4Address : std::array<uint8_t, 4>
{
    IPv4Address() = default;

    IPv4Address(int a, int b, int c, int d)
    {
        (*this)[0] = a;
        (*this)[1] = b;
        (*this)[2] = c;
        (*this)[3] = d;
    }

    uint32_t toInteger() const
    {
        uint32_t result;
        memcpy(&result, data(), size());
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, IPv4Address ip)
    {
        return os << static_cast<int>(ip[0]) << '.'
                  << static_cast<int>(ip[1]) << '.'
                  << static_cast<int>(ip[2]) << '.'
                  << static_cast<int>(ip[3]);
    }
};


struct EthernetHeader
{
    MAC mDestination;
    MAC mSource;
    uint16_t mEtherType = 0x0008;
};


struct IPv4Header
{
    IPv4Header()
    {
        uint8_t example[20] = {
            0x45, 0x00, 0x05, 0xdc, 0x67, 0x4c, 0x00, 0x00, 0x3e, 0x06, 0x01, 0x01,
            0x01, 0x01, 0x01, 0x01,
            0x01, 0x01, 0x02, 0x01
        };
        static_assert(sizeof(*this) == 20, "");
        static_assert(sizeof(example) == sizeof(*this), "");
        memcpy(this, &example[0], sizeof(*this));
    }

    static void mask(uint64_t& a, uint64_t& b)
    {
        union Mask
        {
            std::array<uint8_t, 16> u8;
            uint64_t u64[2];
        };

        Mask mask{};
        mask.u8 =  {{
            0x00, 0xff, 0x00, 0x00, // (ttl) protocol, (checksum)
            0xff, 0xff, 0xff, 0xff,  // source ip
            0xff, 0xff, 0xff, 0xff,  // destination ip
            0xff, 0xff, 0xff, 0xff  // L4Header: source port and destination port
        }};
        a = mask.u64[0];
        b = mask.u64[1];
    }

    uint8_t mVersionAndIHL = (4u << 4) | 5u;
    uint8_t mTypeOfService = 0;
    uint16_t mTotalLength = 1514;
    uint16_t mIdentification = 0;
    uint16_t mFlagsAndFragmentOffset = 0;
    uint8_t mTTL = 255;
    uint8_t mProtocol;
    uint16_t mChecksum =0;
    IPv4Address mSourceIP;
    IPv4Address mDestinationIP;
};


//struct UDPHeader
//{
    //uint16_t mSourcePort;
    //uint16_t mDestinationPort;
    //uint16_t mSize;
    //uint16_t mChecksum;
//};

struct TCPHeader
{
    TCPHeader()
    {
        uint8_t bytes[20] = { 0xec, 0xc6, 0xe0, 0x89, 0x0f, 0x67, 0x67, 0x60, 0x11, 0x51, 0xd9, 0xf9, 0x50, 0x10, 0xff, 0xff, 0xa0, 0x7b, 0x00, 0x00 };
        static_assert(sizeof(*this) == 20, "");
        static_assert(sizeof(*this) == sizeof(bytes), "");
        memcpy(this, &bytes[0], sizeof(*this));
    }
    uint16_t mSourcePort = 0;
    uint16_t mDestinationPort = 0;
    uint16_t mSequenceNumber[2];
    uint16_t mAcknowledgementNumber[2];
    uint16_t mDataOffsetAndFlags = 0;
    uint16_t mWindowSize;
    uint16_t mChecksum = 0;
    uint16_t mUrgentPointer = 0;
};


#ifndef FILTER_BPF
#define FILTER_BPF 0
#endif


#ifndef FILTER_NATIVE
#define FILTER_NATIVE 0
#endif


#ifndef FILTER_VECTOR
#define FILTER_VECTOR 0
#endif


static_assert(FILTER_BPF ^ FILTER_NATIVE ^ FILTER_VECTOR, "");

#if FILTER_BPF
#include "pcap.h"

struct Filter
{
    static std::string get_bpffilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
    {
        assert(protocol == 6); // assume TCP
        // ip src 10.10.1.2 && ip dst 10.10.1.1 && tcp src port 57481 && tcp dst port 60614
        std::stringstream ss;
        ss
            << "ip src " << src_ip
            << " && ip dst " << dst_ip
            << " && src port " << src_port
            << " && dst port " << dst_port
            ;
            (void)src_port;//<< " && tcp src port " << src_port
            (void)dst_port;//<< " && tcp dst port " << dst_port
        return ss.str();
    }

    Filter(std::string bpf_filter)
    {
        //std::cout << bpf_filter << std::endl;
        using DummyInterface = std::unique_ptr<pcap_t, decltype(&pcap_close)>;

        DummyInterface dummy_interface(pcap_open_dead(DLT_EN10MB, 1518), &pcap_close);

        if (!dummy_interface)
        {
            throw std::runtime_error("Failed to open pcap dummy interface");
        }

        auto result = pcap_compile(dummy_interface.get(), &mProgram, bpf_filter.c_str(), 1, 0xff000000);
        if (result != 0)
        {
            std::cout << "pcap_geterr: [" << pcap_geterr(dummy_interface.get()) << "]" << std::endl;
            throw std::runtime_error("pcap_compile failed. Filter=" + bpf_filter);
        }
    }

    Filter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
        Filter(get_bpffilter(protocol, src_ip, dst_ip, src_port, dst_port))
    {
    }

    bool match(const uint8_t* data, uint32_t length) const
    {
        return bpf_filter(mProgram.bf_insns, const_cast<uint8_t*>(data), length, length);
    }
    bpf_program mProgram;
};
#elif FILTER_NATIVE
struct Filter
{
    Filter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
        mProtocol(protocol),
        mSourceIP(src_ip),
        mDestinationIP(dst_ip),
        mSourcePort(src_port),
        mDestinationPort(dst_port)
    {
    }

    bool match(const uint8_t* packet_data, uint32_t /*len*/) const
    {
        const auto& ip_header = *reinterpret_cast<const IPv4Header*>(packet_data + sizeof(EthernetHeader));
        const auto& tcp_header = *reinterpret_cast<const TCPHeader*>(packet_data + sizeof(EthernetHeader) + sizeof(IPv4Header));
        #define TRACE(a, b) //std::cout << #a << "(" << (a) << ") == " << #b << "(" << (b) << "): " << (a == b) << std::endl;
        TRACE(int(ip_header.mProtocol), int(mProtocol));
        TRACE(ip_header.mSourceIP, mSourceIP);
        TRACE(ip_header.mDestinationIP, mDestinationIP);
        TRACE(tcp_header.mSourcePort, mSourcePort);
        TRACE(tcp_header.mDestinationPort, mDestinationPort);

        return ip_header.mProtocol == mProtocol
                && ip_header.mSourceIP == mSourceIP
                && ip_header.mDestinationIP == mDestinationIP
                && tcp_header.mSourcePort == mSourcePort
                && tcp_header.mDestinationPort == mDestinationPort;
    }

    uint8_t mProtocol;
    IPv4Address mSourceIP;
    IPv4Address mDestinationIP;
    uint16_t mSourcePort;
    uint16_t mDestinationPort;
};
#elif FILTER_VECTOR
struct Filter
{
    Filter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
    {
        // Imagine
        struct TransportHeader
        {
            uint8_t ttl;
            uint8_t protocol;
            uint16_t checksum;
            IPv4Address src_ip = IPv4Address();
            IPv4Address dst_ip = IPv4Address();
            uint16_t src_port = 0;
            uint16_t dst_port = 0;
        };

        auto h = TransportHeader();
        h.protocol = protocol;
        h.src_ip = src_ip;
        h.dst_ip = dst_ip;
        h.src_port = src_port;
        h.dst_port = dst_port;


        field_ = _mm_loadu_si128((__m128i*)&h);

        uint8_t mask_bytes[16] = {
            0x00, 0xff, 0x00, 0x00, // ttl, protocol and checksum
            0xff, 0xff, 0xff, 0xff, // source ip
            0xff, 0xff, 0xff, 0xff, // destination ip
            0xff, 0xff, 0xff, 0xff  // source and destination ports
        };
        mask_ = _mm_loadu_si128((__m128i*)&mask_bytes[0]);
    }

    bool match(const uint8_t* packet_data, uint32_t /*len*/) const
    {
        enum { offset = sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(uint16_t) + sizeof(uint16_t) - sizeof(mask_) };

        __m128i mask_result = _mm_cmpeq_epi32(
            field_,
            _mm_and_si128(
                mask_,
                _mm_loadu_si128((__m128i*)(packet_data + offset)))
        );

        __m128i compare_result = _mm_cmpeq_epi8(mask_result, _mm_setzero_si128());
        return _mm_testz_si128(compare_result, compare_result);
    }

    __m128i field_;
    __m128i mask_;
};

#endif


struct Header
{
    Header() = default;

    Header(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
    {
        mIPv4Header.mProtocol = protocol;
        mIPv4Header.mSourceIP = src_ip;
        mIPv4Header.mDestinationIP = dst_ip;
        mNetworkHeader.mSourcePort = src_port;
        mNetworkHeader.mDestinationPort = dst_port;
        static_assert(sizeof(*this) == sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(TCPHeader), "");
    }

    //Header(uint16_t src_port, uint16_t dst_port)
    //{
        //mNetworkHeader.mSourcePort = src_port;
        //mNetworkHeader.mDestinationPort = dst_port;
    //}

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
    Filter mFilter;
};

volatile const unsigned volatile_zero = 0;



struct rdtsc_clock
{
    using time_point = std::uint64_t;

    static time_point now()
    {
        std::uint32_t hi, lo;
        __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
        return (static_cast<time_point>(hi) << 32) | static_cast<time_point>(lo);
    }
};

using Clock = rdtsc_clock;

int64_t get_frequency_mhz()
{
    static auto frequency_mhz = []
    {
        using HRC_t = std::chrono::high_resolution_clock;
        auto c1 = Clock::now();
        auto t1 = HRC_t::now();
        auto now = t1;
        while (now < t1 + std::chrono::milliseconds(10))
        {
            now = HRC_t::now();
        }
        return 1000 * (Clock::now() - c1) / (now - t1).count();
    }();
    return frequency_mhz;
}

double get_frequency_ghz()
{
    static auto frequency_ghz = get_frequency_mhz() / 1000.0;
    return frequency_ghz;
}

int64_t get_frequency_hz()
{
    static auto frequency_hz = get_frequency_mhz() * 1000000;
    return frequency_hz;
}



struct Packet : Header
{
    using Header::Header;
    char bytes[3 * 512 - sizeof(Header)];
}__attribute__((aligned(512)));





template<uint32_t prefetch>
void test(std::vector<Packet>& packets, std::vector<Flow>& flows, uint64_t* matches)
{
    uint32_t num_flows = flows.size();

    auto start_time = Clock::now();

    for (auto i = 0ul; i != packets.size(); ++i)
    {
        Packet& packet = packets[i];

        if (prefetch > 0 && i + prefetch < packets.size())
        {
            __builtin_prefetch(packets[i + prefetch].data(), 0, 0);
        }

        for (auto flow_index = 0ul; flow_index != num_flows; ++flow_index)
        {
            matches[flow_index] += flows[flow_index].match(packet.data(), packet.size());
        }
    }

    auto elapsed_time = Clock::now() - start_time;

    auto cycles_per_packet = 1.0 * elapsed_time / packets.size();
    auto ns_per_packet = cycles_per_packet / get_frequency_ghz();
    auto packet_rate = 1e9 / ns_per_packet / 1000000;

    auto packet_rate_rounded = int(0.5 + packet_rate);

    std::cout
            << " #FLOWS="        << std::setw(3) << std::left << num_flows
            << " PREFETCH="        << prefetch
            << " MPPS="     << std::setw(6) << std::left << packet_rate_rounded
            << " (" << num_flows * packet_rate_rounded << " million filter comparisons per second)"
            ;

    #if 1
    std::cout << " (verify-matches:";
    for (auto i = 0ul; i != num_flows; ++i)
    {
        if (i > 0) std::cout << ',';
        std::cout << int(0.5 + 100.0 * matches[i] / packets.size()) << '%';
    }
    std::cout << ")";
    #endif

}



template<int prefetch>
void run2(uint32_t num_packets, uint32_t num_flows)
{
    std::vector<Packet> packets;
    packets.reserve(num_packets);

    std::vector<Flow> flows;
    flows.reserve(num_flows);

    uint16_t src_port = 0xabab;
    uint16_t dst_port = 0xcdcd;

    for (auto i = 1ul; i <= num_packets; ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1 + i % num_flows);
        IPv4Address dst_ip(1, 1, 2, 1 + i % num_flows);
        packets.emplace_back(6, src_ip, dst_ip, src_port, dst_port);
    }

    std::random_shuffle(packets.begin(), packets.end());



    for (auto i = 0ul; i < num_flows; ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1 + i % num_flows);
        IPv4Address dst_ip(1, 1, 2, 1 + i % num_flows);
        flows.emplace_back(6, src_ip, dst_ip, src_port, dst_port);
    }

    std::vector<uint64_t> matches(num_flows);
    test<prefetch>(packets, flows, matches.data());
    std::cout << std::endl;

}



template<int prefetch>
void run(uint32_t num_packets = 400 * 1000)
{
    run2<prefetch>(num_packets, 1);
    run2<prefetch>(num_packets, 10);
    run2<prefetch>(num_packets, 20);
    run2<prefetch>(num_packets, 50);
    run2<prefetch>(num_packets, 80);
    std::cout << std::endl;
}


int main()
{

    run<0>();
    run<2>();
    run<4>();
}
