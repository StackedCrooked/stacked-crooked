#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>
#include <x86intrin.h>

#if 0

struct MAC : std::array<uint8_t, 6>
{
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
};

struct EthernetHeader
{
    MAC mDestination;
    MAC mSource;
    uint16_t mEtherType;
};


struct IPv4Header
{
    std::array<uint8_t, 12> mStuff;
    IPv4Address mSource;
    IPv4Address mDestination;
};


struct UDPHeader
{
    uint16_t mSourcePort;
    uint16_t mDestinationPort;
    uint16_t mSize;
    uint16_t mChecksum;
};


struct Filter
{
    void set(uint8_t protnum, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
    {
        // Imagine
        struct TransportHeader
        {
            uint8_t unused[3] = { 0, 0, 0 };
            uint8_t protocol = 0;
            IPv4Address src_ip = IPv4Address();
            IPv4Address dst_ip = IPv4Address();
            uint16_t src_port = 0;
            uint16_t dst_port = 0;
        };

        auto h = TransportHeader();
        h.protocol = protnum;
        h.src_ip = src_ip;
        h.dst_ip = dst_ip;
        h.src_port = src_port;
        h.dst_port = dst_port;


        field_ = _mm_loadu_si128((__m128i*)&h.unused[0]);

        uint32_t mask[4] = { 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff };
        mask_ = _mm_loadu_si128((__m128i*)&mask[0]);
    }

    bool match(const uint8_t* packet_data) const
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


struct Header
{
    Header() = default;

    Header(IPv4Address src, IPv4Address dst, uint16_t src_port, uint16_t dst_port)
    {
        mIPv4Header.mSource = src;
        mIPv4Header.mDestination = dst;
        mNetworkHeader.mSourcePort = src_port;
        mNetworkHeader.mDestinationPort = dst_port;
    }

    Header(uint16_t src_port, uint16_t dst_port)
    {
        mNetworkHeader.mSourcePort = src_port;
        mNetworkHeader.mDestinationPort = dst_port;
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
    UDPHeader mNetworkHeader = UDPHeader();
};


struct Flow
{
    Flow() = default;

    Flow(uint8_t protocol, IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port)
    {
        mFilter.set(protocol, source_ip, target_ip, src_port, dst_port);
    }

    bool match(const uint8_t* frame_bytes, int /*len*/)
    {
        // avoid unpredictable branch here.
        return mFilter.match(frame_bytes);
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
void test(uint32_t num_packets, uint32_t num_flows)
{

    std::vector<Packet> packets;
    packets.reserve(num_packets);

    std::vector<Flow> flows;
    flows.reserve(num_flows);

    uint16_t src_port = 1024;
    uint16_t dst_port = 1024;


    for (auto i = 0ul; i < flows.capacity(); ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1 + i);
        IPv4Address dst_ip(1, 1, 2, 1 + i);
        Flow flow(0, src_ip, dst_ip, src_port, dst_port);
        flows.push_back(flow);
    }

    for (auto i = 1ul; i <= packets.capacity(); ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1 + i % flows.size());
        IPv4Address dst_ip(1, 1, 2, 1 + i % flows.size());
        packets.emplace_back(src_ip, dst_ip, src_port, dst_port);
    }


    std::random_shuffle(packets.begin(), packets.end());

    auto start_time = Clock::now();

    switch (prefetch)
    {
        case 0:
        {
            break;
        }
        case 1:
        {
            __builtin_prefetch(packets[0].data(), 0, 0);
            break;
        }
        case 2:
        {
            __builtin_prefetch(packets[0].data(), 0, 0);
            __builtin_prefetch(packets[1].data(), 0, 0);
            break;
        }
        case 3:
        {
            __builtin_prefetch(packets[0].data(), 0, 0);
            __builtin_prefetch(packets[1].data(), 0, 0);
            __builtin_prefetch(packets[2].data(), 0, 0);
            break;
        }
        case 4:
        {
            __builtin_prefetch(packets[0].data(), 0, 0);
            __builtin_prefetch(packets[1].data(), 0, 0);
            __builtin_prefetch(packets[2].data(), 0, 0);
            __builtin_prefetch(packets[3].data(), 0, 0);
            break;
        }
        case 6:
        {
            __builtin_prefetch(packets[0].data(), 0, 0);
            __builtin_prefetch(packets[1].data(), 0, 0);
            __builtin_prefetch(packets[2].data(), 0, 0);
            __builtin_prefetch(packets[3].data(), 0, 0);
            __builtin_prefetch(packets[4].data(), 0, 0);
            __builtin_prefetch(packets[5].data(), 0, 0);
            break;
        }
        case 8:
        {
            __builtin_prefetch(packets[0].data(), 0, 0);
            __builtin_prefetch(packets[1].data(), 0, 0);
            __builtin_prefetch(packets[2].data(), 0, 0);
            __builtin_prefetch(packets[3].data(), 0, 0);
            __builtin_prefetch(packets[4].data(), 0, 0);
            __builtin_prefetch(packets[5].data(), 0, 0);
            __builtin_prefetch(packets[6].data(), 0, 0);
            __builtin_prefetch(packets[7].data(), 0, 0);
            break;
        }
        case 12:
        {
            __builtin_prefetch(packets[0].data(), 0, 0);
            __builtin_prefetch(packets[1].data(), 0, 0);
            __builtin_prefetch(packets[2].data(), 0, 0);
            __builtin_prefetch(packets[3].data(), 0, 0);
            __builtin_prefetch(packets[4].data(), 0, 0);
            __builtin_prefetch(packets[5].data(), 0, 0);
            __builtin_prefetch(packets[6].data(), 0, 0);
            __builtin_prefetch(packets[7].data(), 0, 0);
            __builtin_prefetch(packets[8].data(), 0, 0);
            __builtin_prefetch(packets[9].data(), 0, 0);
            __builtin_prefetch(packets[10].data(), 0, 0);
            __builtin_prefetch(packets[11].data(), 0, 0);
            break;
        }
        case 16:
        {
            __builtin_prefetch(packets[0].data(), 0, 0);
            __builtin_prefetch(packets[1].data(), 0, 0);
            __builtin_prefetch(packets[2].data(), 0, 0);
            __builtin_prefetch(packets[3].data(), 0, 0);
            __builtin_prefetch(packets[4].data(), 0, 0);
            __builtin_prefetch(packets[5].data(), 0, 0);
            __builtin_prefetch(packets[6].data(), 0, 0);
            __builtin_prefetch(packets[7].data(), 0, 0);
            __builtin_prefetch(packets[8].data(), 0, 0);
            __builtin_prefetch(packets[9].data(), 0, 0);
            __builtin_prefetch(packets[10].data(), 0, 0);
            __builtin_prefetch(packets[11].data(), 0, 0);
            __builtin_prefetch(packets[12].data(), 0, 0);
            __builtin_prefetch(packets[13].data(), 0, 0);
            __builtin_prefetch(packets[14].data(), 0, 0);
            __builtin_prefetch(packets[15].data(), 0, 0);
            break;
        }
        case 32:
        {
            __builtin_prefetch(packets[0].data(), 0, 0);
            __builtin_prefetch(packets[1].data(), 0, 0);
            __builtin_prefetch(packets[2].data(), 0, 0);
            __builtin_prefetch(packets[3].data(), 0, 0);
            __builtin_prefetch(packets[4].data(), 0, 0);
            __builtin_prefetch(packets[5].data(), 0, 0);
            __builtin_prefetch(packets[6].data(), 0, 0);
            __builtin_prefetch(packets[7].data(), 0, 0);
            __builtin_prefetch(packets[8].data(), 0, 0);
            __builtin_prefetch(packets[9].data(), 0, 0);
            __builtin_prefetch(packets[10].data(), 0, 0);
            __builtin_prefetch(packets[11].data(), 0, 0);
            __builtin_prefetch(packets[12].data(), 0, 0);
            __builtin_prefetch(packets[13].data(), 0, 0);
            __builtin_prefetch(packets[14].data(), 0, 0);
            __builtin_prefetch(packets[15].data(), 0, 0);


            __builtin_prefetch(packets[16 +  0].data(), 0, 0);
            __builtin_prefetch(packets[16 +  1].data(), 0, 0);
            __builtin_prefetch(packets[16 +  2].data(), 0, 0);
            __builtin_prefetch(packets[16 +  3].data(), 0, 0);
            __builtin_prefetch(packets[16 +  4].data(), 0, 0);
            __builtin_prefetch(packets[16 +  5].data(), 0, 0);
            __builtin_prefetch(packets[16 +  6].data(), 0, 0);
            __builtin_prefetch(packets[16 +  7].data(), 0, 0);
            __builtin_prefetch(packets[16 +  8].data(), 0, 0);
            __builtin_prefetch(packets[16 +  9].data(), 0, 0);
            __builtin_prefetch(packets[16 + 10].data(), 0, 0);
            __builtin_prefetch(packets[16 + 11].data(), 0, 0);
            __builtin_prefetch(packets[16 + 12].data(), 0, 0);
            __builtin_prefetch(packets[16 + 13].data(), 0, 0);
            __builtin_prefetch(packets[16 + 14].data(), 0, 0);
            __builtin_prefetch(packets[16 + 15].data(), 0, 0);
            break;
        }
        case 64:
        {
            __builtin_prefetch(packets[0].data(), 0, 0);
            __builtin_prefetch(packets[1].data(), 0, 0);
            __builtin_prefetch(packets[2].data(), 0, 0);
            __builtin_prefetch(packets[3].data(), 0, 0);
            __builtin_prefetch(packets[4].data(), 0, 0);
            __builtin_prefetch(packets[5].data(), 0, 0);
            __builtin_prefetch(packets[6].data(), 0, 0);
            __builtin_prefetch(packets[7].data(), 0, 0);
            __builtin_prefetch(packets[8].data(), 0, 0);
            __builtin_prefetch(packets[9].data(), 0, 0);
            __builtin_prefetch(packets[10].data(), 0, 0);
            __builtin_prefetch(packets[11].data(), 0, 0);
            __builtin_prefetch(packets[12].data(), 0, 0);
            __builtin_prefetch(packets[13].data(), 0, 0);
            __builtin_prefetch(packets[14].data(), 0, 0);
            __builtin_prefetch(packets[15].data(), 0, 0);


            __builtin_prefetch(packets[16 +  0].data(), 0, 0);
            __builtin_prefetch(packets[16 +  1].data(), 0, 0);
            __builtin_prefetch(packets[16 +  2].data(), 0, 0);
            __builtin_prefetch(packets[16 +  3].data(), 0, 0);
            __builtin_prefetch(packets[16 +  4].data(), 0, 0);
            __builtin_prefetch(packets[16 +  5].data(), 0, 0);
            __builtin_prefetch(packets[16 +  6].data(), 0, 0);
            __builtin_prefetch(packets[16 +  7].data(), 0, 0);
            __builtin_prefetch(packets[16 +  8].data(), 0, 0);
            __builtin_prefetch(packets[16 +  9].data(), 0, 0);
            __builtin_prefetch(packets[16 + 10].data(), 0, 0);
            __builtin_prefetch(packets[16 + 11].data(), 0, 0);
            __builtin_prefetch(packets[16 + 12].data(), 0, 0);
            __builtin_prefetch(packets[16 + 13].data(), 0, 0);
            __builtin_prefetch(packets[16 + 14].data(), 0, 0);
            __builtin_prefetch(packets[16 + 15].data(), 0, 0);


            __builtin_prefetch(packets[32 +  0].data(), 0, 0);
            __builtin_prefetch(packets[32 +  1].data(), 0, 0);
            __builtin_prefetch(packets[32 +  2].data(), 0, 0);
            __builtin_prefetch(packets[32 +  3].data(), 0, 0);
            __builtin_prefetch(packets[32 +  4].data(), 0, 0);
            __builtin_prefetch(packets[32 +  5].data(), 0, 0);
            __builtin_prefetch(packets[32 +  6].data(), 0, 0);
            __builtin_prefetch(packets[32 +  7].data(), 0, 0);
            __builtin_prefetch(packets[32 +  8].data(), 0, 0);
            __builtin_prefetch(packets[32 +  9].data(), 0, 0);
            __builtin_prefetch(packets[32 + 10].data(), 0, 0);
            __builtin_prefetch(packets[32 + 11].data(), 0, 0);
            __builtin_prefetch(packets[32 + 12].data(), 0, 0);
            __builtin_prefetch(packets[32 + 13].data(), 0, 0);
            __builtin_prefetch(packets[32 + 14].data(), 0, 0);
            __builtin_prefetch(packets[32 + 15].data(), 0, 0);


            __builtin_prefetch(packets[48 +  0].data(), 0, 0);
            __builtin_prefetch(packets[48 +  1].data(), 0, 0);
            __builtin_prefetch(packets[48 +  2].data(), 0, 0);
            __builtin_prefetch(packets[48 +  3].data(), 0, 0);
            __builtin_prefetch(packets[48 +  4].data(), 0, 0);
            __builtin_prefetch(packets[48 +  5].data(), 0, 0);
            __builtin_prefetch(packets[48 +  6].data(), 0, 0);
            __builtin_prefetch(packets[48 +  7].data(), 0, 0);
            __builtin_prefetch(packets[48 +  8].data(), 0, 0);
            __builtin_prefetch(packets[48 +  9].data(), 0, 0);
            __builtin_prefetch(packets[48 + 10].data(), 0, 0);
            __builtin_prefetch(packets[48 + 11].data(), 0, 0);
            __builtin_prefetch(packets[48 + 12].data(), 0, 0);
            __builtin_prefetch(packets[48 + 13].data(), 0, 0);
            __builtin_prefetch(packets[48 + 14].data(), 0, 0);
            __builtin_prefetch(packets[48 + 15].data(), 0, 0);
            break;
        }
    }

    std::vector<uint64_t> matches(flows.size());

    for (const Packet& packet : packets)
    {
        if (prefetch > 0)
        {
            __builtin_prefetch(packet.data() + prefetch * sizeof(Packet), 0, 0);
        }

        auto len = flows.size();
        auto flow_ptr = flows.data();
        auto match_ptr = matches.data();



        while (len >= 4)
        {
            match_ptr[0] += flow_ptr[0].match(packet.data(), packet.size());
            match_ptr[1] += flow_ptr[1].match(packet.data(), packet.size());
            match_ptr[2] += flow_ptr[2].match(packet.data(), packet.size());
            match_ptr[3] += flow_ptr[3].match(packet.data(), packet.size());

            len -= 4;
            flow_ptr += 4;
            match_ptr += 4;
        }

        if (len >= 2)
        {
            match_ptr[0] += flow_ptr[0].match(packet.data(), packet.size());
            match_ptr[1] += flow_ptr[1].match(packet.data(), packet.size());
            len -= 2;
            flow_ptr += 2;
            match_ptr += 2;
        }

        if (len == 1)
        {
            match_ptr[0] += flow_ptr[0].match(packet.data(), packet.size());
        }
    }

    auto elapsed_time = Clock::now() - start_time;

    auto cycles_per_packet = 1.0 * elapsed_time / packets.size();
    auto ns_per_packet = cycles_per_packet / get_frequency_ghz();
    auto packet_rate = 1e9 / ns_per_packet / 1000000;

    auto packet_rate_rounded = int(0.5 + packet_rate);

    std::cout
            << " #FLOWS="        << std::setw(2) << std::left << flows.size()
            << " PREFETCH="        << prefetch
            << " MPPS="     << std::setw(6) << std::left << packet_rate_rounded
            << " (" << flows.size() * packet_rate_rounded << " million filter comparisons per second)"
            ;

    #if 1
    std::cout << "  (verify-matches:";
    for (auto i = 0ul; i != flows.size(); ++i)
    {
        if (i > 0) std::cout << ',';
        std::cout << int(0.5 + 100.0 * matches[i]  / packets.size());
    }
    std::cout << ")";
    #endif
    std::cout << std::endl;

}


int main()
{
    auto num_packets = 8 * 1000 * 1000;
    std::array<int, 8> flow_counts = {{ 64, 128, 512, 1024, 2048 }};
    for (int num_flows : flow_counts)
    {
        test<0>(num_packets, num_flows);
        test<1>(num_packets, num_flows);
        test<2>(num_packets, num_flows);
        test<4>(num_packets, num_flows);
        test<8>(num_packets, num_flows);
        test<16>(num_packets, num_flows);
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


struct BpfFilter::Impl {
    Impl(const std::string & myFilterString) : bpfProg() {
        // Note: DLT_EN10MB is also correct for Gbit cards and the like
        pcap_t * dummy_interface = pcap_open_dead(DLT_EN10MB, 1518);
        if (!dummy_interface) {
            EXCENTIS_LOG(Critical) << "Failed to open dummy PCAP interface.";
            throw std::runtime_error("Failed to open dummy interface");
        }

        if (pcap_compile(dummy_interface, &(this->bpfProg), myFilterString.c_str(), 1, 0xff000000)/* < 0 */) {
            // --- Cleanup the code
            std::string errorMessage(pcap_geterr(dummy_interface));
            pcap_close (dummy_interface);

            EXCENTIS_LOG(Error) << "PCAP filter compilation failed: " << myFilterString << ". Reason: "
                                     << errorMessage << ".";

            throw std::logic_error(errorMessage);
        }

        pcap_close (dummy_interface);
    }

    ~Impl() throw() {
        pcap_freecode(&(this->bpfProg));
    }

    struct bpf_program bpfProg;

};
#endif



#include "pcap.h"


struct BPFFilter
{
    BPFFilter(const std::string& bpf_filter)
    {
        using DummyInterface = std::unique_ptr<pcap_t, decltype(&pcap_close)>;

        DummyInterface dummy_interface(pcap_open_dead(DLT_EN10MB, 1518), &pcap_close);

        if (!dummy_interface)
        {
            throw std::runtime_error("Failed to open pcap dummy interface");
        }

        if (!pcap_compile(dummy_interface.get(), &mProgram, bpf_filter.c_str(), 1, 0xff000000))
        {
            throw std::runtime_error("pcap_compile failed. Filter=" + bpf_filter);
        }
    }

    ~BPFFilter()
    {
        pcap_freecode(&mProgram);
    }

    bool check(const uint8_t* data, uint32_t length)
    {
        return bpf_filter(mProgram.bf_insns, const_cast<uint8_t*>(data), length, length);
    }

    bpf_program mProgram;
};



const uint8_t tcp_syn[] = {
    0x00, 0x00, 0x00, 0x00,   0x00, 0x22, 0x00, 0x01,
    0x5c, 0x6c, 0xc2, 0x02,   0x08, 0x00, 0x45, 0x00,
    0x00, 0x34, 0x2b, 0x73,   0x00, 0x00, 0x3e, 0x06,
    0x3b, 0x3b, 0x0a, 0x0a,   0x01, 0x02, 0x0a, 0x0a,
    0x01, 0x01, 0xe0, 0x89,   0xec, 0xc6, 0x11, 0x51,
    0xd9, 0x98, 0x00, 0x00,   0x00, 0x00, 0x80, 0x02,
    0xff, 0xff, 0x9c, 0xc6,   0x00, 0x00, 0x02, 0x04,
    0x05, 0xb4, 0x03, 0x03,   0x05, 0x01, 0x04, 0x02,
    0x01, 0x01
};


int main()
{
    // 10.10.1.1 // 10.10.1.2
    // 606014 57481
    BPFFilter filter("tcp");
    std::cout << filter.check(tcp_syn, sizeof(tcp_syn)) << std::endl;
}
