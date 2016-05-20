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
    void set(IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
    {
        struct H
        {
            uint8_t unused[3] = { 0, 0, 0 };
            uint8_t protocol = 0;
            IPv4Address src_ip = IPv4Address();
            IPv4Address dst_ip = IPv4Address();
            uint16_t src_port = 0;
            uint16_t dst_port = 0;
        };

        auto h = H();
        h.src_ip = src_ip;
        h.dst_ip = dst_ip;
        h.src_port = src_port;
        h.dst_port = dst_port;


        field_ = _mm_loadu_si128((__m128i*)&h.unused[0]);

        uint32_t mask[4] = { 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff };
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

    Flow(IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port)
    {
        mFilter.set(source_ip, target_ip, src_port, dst_port);
    }

    void match(const uint8_t* frame_bytes, int /*len*/)
    {
        if (mFilter.match(frame_bytes))
        {
            mProcessed++;
        }
    }

    std::size_t getMatches() const { return mProcessed; }


private:
    Filter mFilter;
    uint64_t mProcessed = 0;
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




void test(int num_packets, int num_flows, int prefetch)
{
    struct Packet : Header
    {
        using Header::Header;
        char bytes[512 * 3 - sizeof(Header)];
    };

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
        Flow flow(src_ip, dst_ip, src_port, dst_port);
        flows.push_back(flow);
    }

    for (auto i = 1ul; i <= packets.capacity(); ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1 + i % flows.size());
        IPv4Address dst_ip(1, 1, 2, 1 + i % flows.size());
        packets.emplace_back(src_ip, dst_ip, src_port, dst_port);
    }


    //std::random_shuffle(packets.begin(), packets.end());

    auto start_time = Clock::now();

    for (auto i = 0ul; i != packets.size(); i += 1)
    {
        __builtin_prefetch(packets[i+prefetch].data(), 0, 0);
        for (Flow& flow : flows)
        {
            flow.match(packets[i].data(), packets[i].size());
        }
    }

    auto elapsed_time = Clock::now() - start_time;

    auto cycles_per_packet = 1.0 * elapsed_time / packets.size();
    auto ns_per_packet = cycles_per_packet / get_frequency_ghz();
    auto packet_rate = 1e9 / ns_per_packet / 1000000;

    std::cout
            << "num_flows: " << flows.size()
            << " prefetch=" << prefetch
            << " cycles_per_packet_per_flow=" << int(0.5 + cycles_per_packet / flows.size())
            << " packet_rate=" << int(0.5 + 10 * packet_rate) / 10.0 << "M/s"
            ;

    #if 1
    std::cout << " (matches:";
    for (Flow& p : flows)
    {
        std::cout
            << " " << int(0.5 + 100.0 * p.getMatches() / packets.size())
            ;
    }
    std::cout << ")";
    #endif
    std::cout << std::endl;

}


int main()
{
    auto num_packets = 1200 * 1024;
    for (auto num_flows = 1; num_flows <= 16; num_flows *= 2)
    {
        for (auto prefetch = 0; prefetch <= 16; prefetch += 2)
        {
            test(num_packets, num_flows, prefetch);
        }
            std::cout << std::endl;
    }
    std::cout << std::endl;
}
