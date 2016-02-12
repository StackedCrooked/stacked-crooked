#include <array>
#include <algorithm>
#include <boost/container/static_vector.hpp>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>


struct MAC : std::array<uint8_t, 6> {};


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


struct NetworkHeader
{
    uint16_t mSourcePort;
    uint16_t mDestinationPort;
};


struct PacketHeader
{
    PacketHeader() = default;

    PacketHeader(IPv4Address src, IPv4Address dst, uint16_t src_port, uint16_t dst_port)
    {
        mIPv4Header.mSource = src;
        mIPv4Header.mDestination = dst;
        mNetworkHeader.mSourcePort = src_port;
        mNetworkHeader.mDestinationPort = dst_port;
    }

    PacketHeader(uint16_t src_port, uint16_t dst_port)
    {
        mNetworkHeader.mSourcePort = src_port;
        mNetworkHeader.mDestinationPort = dst_port;
    }



    std::size_t hash() const
    {
        std::size_t result = 0;
        boost::hash_combine(result, mIPv4Header.mSource.toInteger());
        boost::hash_combine(result, mIPv4Header.mDestination.toInteger());
        const void* p = &mNetworkHeader;
        boost::hash_combine(result, static_cast<const uint32_t*>(p)[0]);
        return result;
    }

	uint8_t* data() { return static_cast<uint8_t*>(static_cast<void*>(this)); }
	uint8_t* begin() { return data(); }
	uint8_t* end() { return data() + sizeof(*this); }

	const uint8_t* data() const { return static_cast<const uint8_t*>(static_cast<const void*>(this)); }
	const uint8_t* begin() const { return data(); }
	const uint8_t* end() const { return data() + sizeof(*this); }

    EthernetHeader mEthernetHeader = EthernetHeader();
    IPv4Header mIPv4Header = IPv4Header();
    NetworkHeader mNetworkHeader = NetworkHeader();
};


struct Packet
{
    const uint8_t* mData;
    uint16_t mSize;
};


struct Segment
{
    enum
    {
        max_packets = 65536,
        max_bytes   = max_packets * 64
    };

    std::array<uint16_t, max_packets> mPacketOffsets;
    std::array<uint8_t, max_bytes> mPacketData;
};



struct Processor
{
    Processor() = default;

    Processor(IPv4Address source_ip, IPv4Address target_ip, uint16_t src_port, uint16_t dst_port)
    {
        mFilter[0] = source_ip.toInteger();
        mFilter[1] = target_ip.toInteger();

        auto u16 = reinterpret_cast<uint16_t*>(&mFilter[2]);
        u16[0] = src_port;
        u16[1] = dst_port;
    }


    std::size_t hash() const
    {
        std::size_t result = 0;
        boost::hash_combine(result, mFilter[0]);
        boost::hash_combine(result, mFilter[1]);
        boost::hash_combine(result, mFilter[2]);
        return result;
    }


    void process()
    {
        mProcessed++;
    }

    bool match(const uint8_t* bytes)
    {
        mChecked++;
        auto start = bytes + sizeof(EthernetHeader) + sizeof(IPv4Header) - sizeof(IPv4Address) - sizeof(IPv4Address);
    #if 1
        auto filter = (uint32_t*)start;
        #if 0
        std::cout
                << "Incoming=" << filter[0] << ' ' << filter[1] << ' '
                << ' ' << ((uint16_t*)&filter[2])[0]
                << ' ' << ((uint16_t*)&filter[2])[1]
                << " Filter==" << mFilter[0]
                << ' ' << ((uint16_t*)&mFilter[2])[0]
                << ' ' << ((uint16_t*)&mFilter[2])[1]
                << '\n';
        #endif
        return (filter[2] == mFilter[2])
             & (filter[1] == mFilter[1])
             & (filter[0] == mFilter[0]);
    #else
        return !memcmp(start, mFilter.data(), mFilter.size());
    #endif
    }


    // src_ip, dst_ip, src_and_dst_ports
    using Filter = std::array<uint32_t, 3>;

    Filter mFilter = Filter();
    uint64_t mProcessed = 0;
    uint64_t mChecked = 0;
};


// TODO: Test this hash function and check if unrolling can help.
std::size_t hash(uint8_t* offsets, const uint8_t** values, int* lengths, int len)
{
    std::size_t result = 0;
    for (auto i = 0; i != len; ++i)
    {
        auto b = values[i] + offsets[i];
        auto l = lengths[i];
        for (auto i = 0; i != l; ++i)
        {
            boost::hash_combine(result, b[i]);
        }
    }
    return result;
}


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

void test_one_processor()
{
    std::vector<PacketHeader> headers;

    std::vector<Processor> processors;

    for (auto i = 1024u; i <= 1024u + volatile_zero; ++i)
    {
        headers.emplace_back(IPv4Address(1, 1, 1, i % 4), IPv4Address(1, 1, 1, i % 4), i, i);
        processors.emplace_back(IPv4Address(1, 1, 1, i % 4), IPv4Address(1, 1, 1, i % 4), i, i);
    }

    headers.resize(10000000, headers.front());


    for (auto i = 0; i != 2; ++i)
    {
        auto start_time = Clock::now();
        for (auto i = 0u; i != headers.size(); ++i)
        {
            for (Processor& p : processors)
            {
                if (p.match(headers[i].data()))
                {
                    p.process();
                    //continue;
                }
            }
        }
        auto elapsed_time = Clock::now() - start_time;
        std::cout << "Total=" << elapsed_time
            << " time/packet=" << (1.0 * elapsed_time / headers.size())
            << " time/packet/filter=" << (1.0 * elapsed_time / headers.size() / processors.size()) << '\n';

    }

    for (Processor& p : processors)
    {
        std::cout
            << p.mProcessed << '/' << p.mChecked
            << " " << int(0.5 + 100.0 * p.mProcessed / p.mChecked) << "%\n";
    }
}

void test_multiple_processors()
{
    std::vector<PacketHeader> headers;

    std::vector<Processor> processors;


    for (auto i = 0; i <= 10000000 / 16; ++i)
    {
        for (auto i = 0; i != 16; ++i)
        {
            headers.emplace_back(IPv4Address(1, 1, 1, i % 4), IPv4Address(1, 1, 1, i % 4), i, i);
            if (processors.size() < 16)
            {
                processors.emplace_back(IPv4Address(1, 1, 1, i % 4), IPv4Address(1, 1, 1, i % 4), i, i);
            }
        }
    }

    auto procs = std::array<boost::container::static_vector<uint16_t, 1>, 16>();

    for (Processor& proc : processors)
    {
        auto index = &proc - processors.data();
        procs[proc.hash() % procs.size()].push_back(index);
    }



    for (auto i = 0; i != 2; ++i)
    {
        auto start_time = Clock::now();
        for (auto i = 0u; i != headers.size(); ++i)
        {
            PacketHeader& h = headers[i];
            for (uint32_t index : procs[h.hash() % procs.size()])
            {
                auto& p = processors[index];
                if (p.match(headers[i].data()))
                {
                    p.process();
                }
            }
        }

        auto elapsed_time = Clock::now() - start_time;
        std::cout << "Total=" << elapsed_time
            << " time/packet=" << (1.0 * elapsed_time / headers.size())
            << " time/packet/filter=" << (1.0 * elapsed_time / headers.size() / processors.size()) << '\n';

    }

    for (Processor& p : processors)
    {
        std::cout
            << p.mProcessed << '/' << p.mChecked
            << " " << int(0.5 + 100.0 * p.mProcessed / p.mChecked) << "%\n";
    }
}



int main()
{
    test_one_processor();
    test_multiple_processors();
}
