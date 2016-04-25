
#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))


#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>


struct Filter
{
    void add(uint8_t value, int offset)
    {
        mItems.push_back(Item(value, offset));
    }

    void add(uint16_t value, int offset)
    {
        mItems.push_back(Item(value, offset));
    }

    void add(uint32_t value, int offset)
    {
        mItems.push_back(Item(value, offset));
    }

    bool match(const uint8_t* bytes, int len) const
    {
        for (const Item& item : mItems)
        {
            auto i = &item - mItems.data(); (void)i;
            if (!item.match(bytes, len))
            {
                return false;
            }
        }
        return true;
    }

private:
    struct Item
    {
        Item(uint32_t value, int offset) :
            storage_(value),
            field_offset_(offset),
            field_length_(sizeof(value))
        {
        }

        Item(uint16_t value, int offset) :
            storage_(value),
            field_offset_(offset),
            field_length_(sizeof(value))
        {
        }

        Item(uint8_t value, int offset) :
            storage_(value),
            field_offset_(offset),
            field_length_(sizeof(value))
        {
        }

        bool match(const uint8_t* frame, unsigned size) const
        {
            if (UNLIKELY(field_length_ + field_offset_ > size))
            {
               return false;
            }

            auto input = frame + field_offset_;

            if (field_length_ == 4) return get_field_32() == *(uint32_t*)(input);
            if (field_length_ == 2) return get_field_16() == *(uint16_t*)(input);
            return get_field_8() == *input;
        }

        const uint8_t* get_field() const { return static_cast<const uint8_t*>(static_cast<const void*>(&storage_)); }
        uint16_t get_field_8() const { return *static_cast<const uint8_t*>(static_cast<const void*>(&storage_)); }
        uint16_t get_field_16() const { const void* ptr = &storage_; return *static_cast<const uint16_t*>(ptr); }
        uint16_t get_field_16(std::size_t i) const { return static_cast<const uint16_t*>(static_cast<const void*>(&storage_))[i]; }
        uint32_t get_field_32() const { return storage_; }

        uint32_t storage_;
        uint16_t field_offset_;
        uint16_t field_length_;
    };

    std::vector<Item> mItems;
};



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


struct Processor
{
    Processor() = default;

    Processor(IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port)
    {
        enum : unsigned
        {
            tuple_offset = sizeof(EthernetHeader) + sizeof(IPv4Header) - 2 * sizeof(IPv4Address)
        };

        unsigned offset = tuple_offset;

        mFilter.add(source_ip.toInteger(), offset); offset += sizeof(source_ip);
        mFilter.add(target_ip.toInteger(), offset); offset += sizeof(target_ip);
        mFilter.add(src_port, offset); offset += sizeof(src_port);

        mFilter.add(dst_port, offset);
    }

    void process(const uint8_t* frame_bytes, int len)
    {
        if (do_process(frame_bytes, len))
        {
            mProcessed++;
        }
    }

    std::size_t getMatches() const { return mProcessed; }

    bool do_process(const uint8_t* frame_bytes, int len)
    {
        return mFilter.match(frame_bytes, len);
    }


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




void test(int num_packets, int num_processors)
{
    struct Packet : Header
    {
        using Header::Header;
        char bytes[1536 - sizeof(Header)];
    };

std::vector<Packet> packets;
    packets.reserve(num_packets);

    std::vector<Processor> processors;
    processors.reserve(num_processors);

    uint16_t src_port = 1024;
    uint16_t dst_port = 1024;


    for (auto i = 0ul; i < processors.capacity(); ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1 + i);
        IPv4Address dst_ip(1, 1, 2, 1 + i);
        Processor proc(src_ip, dst_ip, src_port, dst_port);
        processors.push_back(proc);
    }

    for (auto i = 1ul; i <= packets.capacity(); ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1 + i % processors.size());
        IPv4Address dst_ip(1, 1, 2, 1 + i % processors.size());
        packets.emplace_back(src_ip, dst_ip, src_port, dst_port);
    }


    std::random_shuffle(packets.begin(), packets.end());



    auto total_counter = 0ul;

    auto start_time = Clock::now();
    for (auto i = 0ul; i != packets.size(); i += 2)
    {
        __builtin_prefetch(packets[i+2].data(), 0, 0);
        __builtin_prefetch(packets[i+3].data(), 0, 0);
        total_counter += 2;
        for (Processor& processor : processors)
        {
            processor.process(packets[i+0].data(), packets[i+0].size());
            processor.process(packets[i+1].data(), packets[i+1].size());
        }
    }
    auto elapsed_time = Clock::now() - start_time;

    auto cycles_per_packet = 1.0 * elapsed_time / packets.size();
    auto ns_per_packet = cycles_per_packet / get_frequency_ghz();
    auto packet_rate = 1e9 / ns_per_packet / 1000000;

        std::cout
                << "\nPROCESSOR COUNT: " << processors.size()
                << "\ncycles_per_packet=" << int(0.5 + cycles_per_packet)
                << "\ncycles_per_packet_per_processor=" << int(0.5 + cycles_per_packet / processors.size())
                << "\nns_per_packet=" << int(0.5 + ns_per_packet)
                << "\nns_per_packet_per_processor=" << int(0.5 + ns_per_packet / processors.size())
                << "\npacket_rate=" << int(0.5 + 10 * packet_rate) / 10.0 << "M/s"
                << std::endl;

    for (Processor& p : processors)
    {
        std::cout
            << "processors[" << (&p - processors.data()) << "].matches="
            << int(0.5 + 100.0 * p.getMatches() / packets.size()) << "%"
            << '\n';
    }

    std::cout << std::endl;


}

int main()
{
    auto num_packets = 20 * 1000;
    for (auto num_processors = 1; num_processors <= 16; num_processors *= 2)
    {
        test(num_packets, num_processors);
    }
}
