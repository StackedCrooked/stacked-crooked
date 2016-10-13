#include <boost/functional/hash.hpp>
#include <array>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <typeinfo>
#include <cxxabi.h>
#include <x86intrin.h>
#include "pcap.h"


template<typename T>
inline T Decode(const uint8_t* data)
{
    auto result = T();
    memcpy(&result, data, sizeof(result));
    return result;
}


const char* Demangle(char const * mangled_name)
{
    int st;
    auto demangled = abi::__cxa_demangle(mangled_name, 0, 0, &st);
    return st == 0 ? demangled : mangled_name;
}


template<typename T>
const char* GetTypeName()
{
    return Demangle(typeid(T).name());
}


struct MACAddress
{
    std::array<uint8_t, 6> mData;
};


struct IPv4Address
{
    IPv4Address() : mData() {}

    IPv4Address(int a, int b, int c, int d)
    {
        mData[0] = a;
        mData[1] = b;
        mData[2] = c;
        mData[3] = d;
    }

    uint32_t toInteger() const
    {
        uint32_t result;
        memcpy(&result, mData.data(), mData.size());
        return result;
    }

    friend bool operator==(IPv4Address lhs, IPv4Address rhs)
    {
        return lhs.toInteger() == rhs.toInteger();
    }

    friend std::ostream& operator<<(std::ostream& os, IPv4Address ip)
    {
        return os << static_cast<int>(ip.mData[0]) << '.'
                  << static_cast<int>(ip.mData[1]) << '.'
                  << static_cast<int>(ip.mData[2]) << '.'
                  << static_cast<int>(ip.mData[3]);
    }

    std::array<uint8_t, 4> mData;
};


struct EthernetHeader
{
    MACAddress mDestination;
    MACAddress mSource;
    uint16_t mEtherType = 0x0008;
};


struct IPv4Header
{
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


struct TCPHeader
{
    uint16_t mSourcePort = 0;
    uint16_t mDestinationPort = 0;
    uint16_t mSequenceNumber[2];
    uint16_t mAcknowledgementNumber[2];
    uint16_t mDataOffsetAndFlags = 0;
    uint16_t mWindowSize;
    uint16_t mChecksum = 0;
    uint16_t mUrgentPointer = 0;
};


struct BPFFilter
{
    static std::string make_bpf_string(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
    {
        const char* protocol_str = (protocol == 6) ? "tcp" : "(unknown)";

        std::stringstream ss;
        ss  << "ip src " << src_ip
            << " && ip dst " << dst_ip
            << " && " << protocol_str << " src port " << src_port
            << " && " << protocol_str << " dst port " << dst_port
            ;
        return ss.str();
    }

    BPFFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
    {
        std::string bpf_string = make_bpf_string(protocol, src_ip, dst_ip, src_port, dst_port);

        DummyInterface dummy_interface(pcap_open_dead(DLT_EN10MB, 1518), &pcap_close);

        if (!dummy_interface)
        {
            throw std::runtime_error("Failed to open pcap dummy interface");
        }

        auto result = pcap_compile(dummy_interface.get(), &mProgram, bpf_string.c_str(), 1, 0xff000000);
        if (result != 0)
        {
            std::cout << "pcap_geterr: [" << pcap_geterr(dummy_interface.get()) << "]" << std::endl;
            throw std::runtime_error("pcap_compile failed. Filter=" + bpf_string);
        }
    }

    bool match(const uint8_t* data, uint32_t length) const
    {
        return bpf_filter(mProgram.bf_insns, const_cast<uint8_t*>(data), length, length);
    }

    private:
    using DummyInterface = std::unique_ptr<pcap_t, decltype(&pcap_close)>;
    
    bpf_program mProgram;
};


struct NativeFilter
{
    NativeFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
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

        return    (ip_header.mProtocol == mProtocol)
               && (ip_header.mSourceIP == mSourceIP)
               && (ip_header.mDestinationIP == mDestinationIP)
               && (tcp_header.mSourcePort == mSourcePort)
               && (tcp_header.mDestinationPort == mDestinationPort);
    }

    uint8_t mProtocol;
    IPv4Address mSourceIP;
    IPv4Address mDestinationIP;
    uint16_t mSourcePort;
    uint16_t mDestinationPort;
};


struct VectorFilter
{
    VectorFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
    {
        // Composite of IPv4 + TCP/UDP header fields from TTL to DestinationPort.
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


struct MaskFilter
{
    MaskFilter(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port)
    {
        // Composite of IPv4 + TCP/UDP header fields from TTL to DestinationPort.
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

        static_assert(sizeof(mFields) == sizeof(h), "");

        memcpy(&mFields[0], &h, sizeof(mFields));

        uint8_t mask_bytes[16] = {
            0x00, 0xff, 0x00, 0x00, // ttl, protocol and checksum
            0xff, 0xff, 0xff, 0xff, // source ip
            0xff, 0xff, 0xff, 0xff, // destination ip
            0xff, 0xff, 0xff, 0xff  // source and destination ports
        };

        static_assert(sizeof(mask_bytes) == sizeof(mMasks), "");

        memcpy(&mMasks[0], &mask_bytes[0], sizeof(mMasks));
    }

    bool match(const uint8_t* packet_data, uint32_t /*len*/) const
    {
        enum { offset = sizeof(EthernetHeader) + sizeof(IPv4Header) + sizeof(uint16_t) + sizeof(uint16_t) - sizeof(mMasks) };

        using U64 = std::array<uint64_t, 2>;
        auto u64 = Decode<U64>(packet_data + offset);
    
        return (mFields[0] == (mMasks[0] & u64[0]))
            && (mFields[1] == (mMasks[1] & u64[1]));
    }

    uint64_t mFields[2];
    static uint64_t mMasks[2];
};


uint64_t MaskFilter::mMasks[2];

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
        mFilter(protocol, source_ip, target_ip, src_port, dst_port),
        mHash(0)
    {
        boost::hash_combine(mHash, protocol);
        boost::hash_combine(mHash, source_ip.toInteger());
        boost::hash_combine(mHash, target_ip.toInteger());
        boost::hash_combine(mHash, src_port);
        boost::hash_combine(mHash, dst_port);
    }

    bool match(const uint8_t* frame_bytes, int len) const
    {
        // avoid unpredictable branch here.
        return mFilter.match(frame_bytes, len);
    }

    std::size_t hash() const
    {
        return mHash;
    }

private:
    FilterType mFilter;
    std::size_t mHash;
};



// Packet distance is 1536 (or 512 * 3) bytes.
// This seems to be the default used by pfring in it's
// internal Rx buffer.
struct Packet
{
    Packet() :
        mFullPacketHeader(),
        padding()
    {
    }

    Packet(uint8_t protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port):
        mFullPacketHeader(protocol, src_ip, dst_ip, src_port, dst_port)
    {
    }

    const uint8_t* data() const { return mFullPacketHeader.data(); }
    uint32_t size() const { return mFullPacketHeader.size(); }

    CombinedHeader mFullPacketHeader;
    char padding[3 * 512 - sizeof(CombinedHeader)];
};



template<typename FlowType>
struct Flows
{
    void add_flow(uint8_t protocol, IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port)
    {
        auto flow_index = mFlows.size();
        mFlows.emplace_back(protocol, source_ip, target_ip, src_port, dst_port);
		auto hash = mFlows[flow_index].hash();
        auto bucket_index = hash % mHashTable.size();
        mHashTable[bucket_index].push_back(flow_index);

        #if 0
        if (mHashTable[bucket_index].size() >= 2)
        {
            std::cout << "flow_index=" << flow_index << " hash=" << hash << " bucket_index=" << bucket_index << " bucket_entries=" << mHashTable[bucket_index].size() << std::endl;
        }
        #endif
    }

	void print()
	{
		std::cout << "HashTable:" << std::endl;
		for (auto i = 0u; i != mHashTable.size(); ++i)
		{
			std::cout << " i=" << i << " size=" << mHashTable[i].size() << std::endl;
		}
	}

    std::size_t size() const
    {
        return mFlows.size();
    }

	void match(const Packet& packet, uint64_t* matches)
    {
        std::size_t packet_hash = 0;
        boost::hash_combine(packet_hash, packet.mFullPacketHeader.mIPv4Header.mProtocol);
        boost::hash_combine(packet_hash, packet.mFullPacketHeader.mIPv4Header.mSourceIP.toInteger());
        boost::hash_combine(packet_hash, packet.mFullPacketHeader.mIPv4Header.mDestinationIP.toInteger());
        boost::hash_combine(packet_hash, packet.mFullPacketHeader.mNetworkHeader.mSourcePort);
        boost::hash_combine(packet_hash, packet.mFullPacketHeader.mNetworkHeader.mDestinationPort);

        auto bucket_index = packet_hash % mHashTable.size();
		//std::cout << "packet_hash=" << packet_hash << " bucket_index=" << bucket_index << std::endl;

        std::vector<uint32_t>& flow_indexes = mHashTable[bucket_index];
        for (const uint32_t& flow_index : flow_indexes)
        {
            matches[flow_index] += mFlows[flow_index].match(packet.data(), packet.size());
        }
    }

    std::vector<Flow<FlowType>> mFlows;

    std::array<std::vector<uint32_t>, 1021> mHashTable;
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


template<typename FilterType, uint32_t prefetch>
void run3(std::vector<Packet>& packets, Flows<FilterType>& flows, uint64_t* const matches)
{
    const uint32_t num_flows = flows.size();

    auto start_time = Clock::now();

    for (auto i = 0ul; i != packets.size(); ++i)
    {
        Packet& packet = packets[i];

        if (prefetch > 0)
        {
            __builtin_prefetch(packets[i + prefetch].data(), 0, 0);
        }

		flows.match(packet, matches);
    }

    auto elapsed_time = Clock::now() - start_time;

    auto cycles_per_packet = 1.0 * elapsed_time / packets.size();
    auto ns_per_packet = cycles_per_packet / get_frequency_ghz();
    auto packet_rate = 1e9 / ns_per_packet / 1000000;

    auto packet_rate_rounded = int(0.5 + 100 * packet_rate)/100.0;

    std::cout << std::setw(12) << std::left << GetTypeName<FilterType>()
            << " PREFETCH="        << prefetch
            << " FLOWS=" << std::setw(4) << std::left << num_flows
            << " MPPS="     << std::setw(9) << std::left << packet_rate_rounded
            ;

    #if 1
    std::cout << " (verify-matches:";
    for (auto i = 0ul; i != std::min(num_flows, 20u); ++i)
    {
        if (i > 0) std::cout << ',';
        std::cout << 100.0 * matches[i] / packets.size() << '%';
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

    Flows<FilterType> flows;
    flows.mFlows.reserve(num_flows);

    uint16_t src_port = 0xabab;
    uint16_t dst_port = 0xcdcd;

    for (auto i = 0ul; i < num_packets; ++i)
    {
        for (auto a = 0; a != 8; ++a)
        for (auto b = 0; b != 8; ++b)
        for (auto c = 0; c != 8; ++c)
        for (auto d = 0; d != 8; ++d)
        {
            IPv4Address src_ip(a, b, c, d);
            IPv4Address dst_ip(d, c, b, a);
            packets.emplace_back(6, src_ip, dst_ip, src_port, dst_port);
            if (packets.size() >= num_flows)
            {
                goto shuffle;
            }
            packets.emplace_back(6, src_ip, dst_ip, dst_port, src_port);
            if (packets.size() >= num_flows)
            {
                goto shuffle;
            }
        }
    }

shuffle:
    auto copy = packets;
    while (packets.size() < num_packets)
    {
        packets.insert(packets.end(), copy.begin(), copy.end());
    }
    packets.resize(num_packets);

    for (auto i = 0ul; i < num_flows; ++i)
    {
        for (auto a = 0; a != 8; ++a)
        for (auto b = 0; b != 8; ++b)
        for (auto c = 0; c != 8; ++c)
        for (auto d = 0; d != 8; ++d)
        {
            IPv4Address src_ip(a, b, c, d);
            IPv4Address dst_ip(d, c, b, a);
            flows.add_flow(6, src_ip, dst_ip, src_port, dst_port);
            if (flows.size() >= num_flows)
            {
                goto next;
            }
            flows.add_flow(6, src_ip, dst_ip, dst_port, src_port);
            if (flows.size() >= num_flows)
            {
                goto next;
            }
        }
    }

next:
    std::vector<uint64_t> matches(num_flows);
    run3<FilterType, prefetch>(packets, flows, matches.data());
    std::cout << std::endl;

}




template<typename FilterType>
void run(uint32_t num_packets = 512 * 1024)
{
    int flow_counts[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };

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
    run<BPFFilter>();
    std::cout << std::endl;

//    run<NativeFilter>();
//    std::cout << std::endl;

    run<MaskFilter>();
    std::cout << std::endl;

//    run<VectorFilter>();
//    std::cout << std::endl;
}

