#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>


struct MAC : std::array<uint8_t, 6> {};
struct IP : std::array<uint8_t, 4> {};


struct EthernetHeader
{
    MAC mDestination;
    MAC mSource;
    uint16_t mEtherType;
};


struct IPv4Header
{
    std::array<uint8_t, 12> mStuff;
    IP mSource;
    IP mDestination;
};


struct NetworkHeader
{
    uint16_t mSourcePort;
    uint16_t mDestinationPort;
};


struct PacketHeader
{
    PacketHeader()
    {
        for (auto& byte : *this) byte = (rand() % 10) > 1;
    }
	uint8_t* data() { return static_cast<uint8_t*>(static_cast<void*>(this)); }
	uint8_t* begin() { return data(); }
	uint8_t* end() { return data() + sizeof(*this); }

	const uint8_t* data() const { return static_cast<const uint8_t*>(static_cast<const void*>(this)); }
	const uint8_t* begin() const { return data(); }
	const uint8_t* end() const { return data() + sizeof(*this); }

    EthernetHeader mEthernetHeader;
    IPv4Header mIPv4Header;
    NetworkHeader mNetworkHeader;
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
	Processor() : mCounter(0)
    {
        for (auto& field : mFilter)
        {
            field = (rand() % 10) > 1;
        }
    }
    void process()
    {
        mCounter++;
    }

    bool match(const uint8_t* bytes)
    {
        auto start = bytes + sizeof(EthernetHeader) + sizeof(IPv4Header) - sizeof(IP) - sizeof(IP);
    #if 1
        auto filter = (uint32_t*)start;
        return (filter[0] == mFilter[0])
             && (filter[1] == mFilter[1])
             && (filter[2] == mFilter[2]);
    #else
        return !memcmp(start, mFilter.data(), mFilter.size());
    #endif
    }

    using Filter = std::array<uint32_t, 3>;
    Filter mFilter; // src_ip, dst_ip, src_and_dst_ports
    uint64_t mCounter;
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

int main()
{
    std::srand(time(0));
    std::vector<PacketHeader> headers(1024 * 1024);
    std::vector<Processor> processors(200);

	// Make sure the compiler cannot make any assumptions aobut the
	// contents of the headers.
	memset(headers.data(), volatile_zero, headers.size());
	

    for (auto i = 0; i != 10; ++i)
    {
        auto start_time = Clock::now();
        for (auto i = 0u; i != headers.size(); ++i)
        {
			for (auto& p : processors)
			{
				if (p.match(headers[i].data()))
                {
                    p.process();
                    continue;
                }
			}
        }
        auto elapsed_time = Clock::now() - start_time;
        std::cout << "Total=" << elapsed_time
            << " time/packet=" << (1.0 * elapsed_time / headers.size())
            << " time/packet/filter=" << (1.0 * elapsed_time / headers.size() / processors.size()) << '\n';

    }

	for (auto& p : processors)
	{
		std::cout << p.mCounter << ' ';
	}
}
