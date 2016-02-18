#include "Processor.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <vector>



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
    struct Packet : ::Header
    {
        using ::Header::Header;
        char bytes[1460];
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
    for (auto& packet : packets)
    {
        __builtin_prefetch(packet.data() + 14 + 18 + 1 * sizeof(Packet), 0, 0);
        total_counter++;
        for (Processor& processor : processors)
        {
            processor.process(packet.data(), packet.size());
        }
    }
    auto elapsed_time = Clock::now() - start_time;

    auto cycles_per_packet = 1.0 * elapsed_time / packets.size();
    auto ns_per_packet = cycles_per_packet / get_frequency_ghz();
    auto packet_rate = 1e9 / ns_per_packet / 1000000;

        std::cout
                << "\nprocessors_per_packet=" << processors.size()
                << "\ncycles_per_packet=" << int(0.5 + cycles_per_packet)
                << "\nns_per_packet=" << int(0.5 + ns_per_packet)
                << "\npacket_rate=" << int(0.5 + 10 * packet_rate) / 10.0 << "M/s"
                << "\nns_per_packet_per_processor=" << int(0.5 + ns_per_packet / processors.size())
                << std::endl;

    #if 0
    for (Processor& p : processors)
    {
        std::cout
            << "Processor[" << (&p - processors.data()) << "].matches "
            << int(0.5 + 100.0 * p.getMatches() / packets.size()) << "%"
            << std::endl;
    }
    #endif


}

int main()
{
    auto num_packets = 20 * 1000;
    for (auto num_processors = 1; num_processors <= 128; num_processors *= 2)
    {
        test(num_packets, num_processors);
    }
}
