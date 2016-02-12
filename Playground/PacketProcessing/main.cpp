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
	const uint8_t* data() const { return static_cast<const uint8_t*>(static_cast<const void*>(this)); }

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
	Processor() : mCounter(0) {}
    void process(const uint8_t* bytes)
    {
        if (match(bytes))
        {
            mCounter++;
        }
    }

    bool match(const uint8_t* bytes)
    {
    
        auto start = bytes + sizeof(EthernetHeader) + sizeof(IPv4Header) - sizeof(IP) - sizeof(IP);
        Filter filter;
        memcpy(&filter, start, sizeof(filter));
        return (filter[0] == mFilter[0])
             & (filter[1] == mFilter[1])
             & (filter[2] == mFilter[2]);
    }

    using Filter = std::array<uint32_t, 3>;
    Filter mFilter; // src_ip, dst_ip, src_and_dst_ports
    uint64_t mCounter;
};



std::array<PacketHeader, 4> headers;


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
	// Make sure the compiler cannot make any assumptions aobut the
	// contents of the headers.
	memset(&headers, volatile_zero, sizeof(headers));
	
	Processor p;
	p.mFilter[0] = volatile_zero;
	p.mFilter[1] = volatile_zero;
	p.mFilter[2] = volatile_zero;


	assert(p.mCounter == 0);

	for (auto i = 0; i != headers.size(); ++i)
	{
		p.process(headers[i].data()); // warmup
	}

	auto start_time = Clock::now();
	for (auto i = 0; i != headers.size(); ++i)
	{
		p.process(headers[i].data()); // warmup
	}
	auto elapsed_time = Clock::now() - start_time;
	std::cout << "Total=" << elapsed_time << " time/packet=" << (1.0 * elapsed_time / headers.size()) << '\n';


	for (auto i = 0; i != headers.size(); ++i)
	{
		auto start_time = Clock::now();
		p.process(headers[i].data()); // warmup
		auto elapsed_time = Clock::now() - start_time;
		std::cout << "i=" << i << " cycles=" << elapsed_time << '\n';
	}

	std::cout << "p.mCounter=" << p.mCounter << '\n';

}
