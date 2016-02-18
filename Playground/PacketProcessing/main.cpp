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
    struct Header : ::Header
    {
        using ::Header::Header;
        char bytes[2000];
    };
    std::vector<Header> headers;
    headers.reserve(num_packets);

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

    for (auto i = 1ul; i <= headers.capacity(); ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1 + i % processors.size());
        IPv4Address dst_ip(1, 1, 2, 1 + i % processors.size());
        Header  header(src_ip, dst_ip, src_port, dst_port);
        headers.push_back(header);
    }


    std::random_shuffle(headers.begin(), headers.end());



    auto total_counter = 0ul;

    auto start_time = Clock::now();
    for (const Header& header : headers)
    {
        total_counter++;
        for (Processor& processor : processors)
        {
            processor.process(header.data(), header.size());
        }
    }
    auto elapsed_time = Clock::now() - start_time;

    auto cycles_per_packet = 1.0 * elapsed_time / headers.size();
    auto ns_per_packet = cycles_per_packet / get_frequency_ghz();
    auto packet_rate = 1e9 / ns_per_packet / 1000000;

        std::cout
                << "\nprocessors_per_packet=" << processors.size()
                << "\ncycles_per_packet=" << int(0.5 + cycles_per_packet)
                << "\nns_per_packet=" << int(0.5 + ns_per_packet)
                << "\nns_per_packet_per_processor=" << int(0.5 + ns_per_packet / processors.size())
                << "\npacket_rate=" << int(0.5 + 10 * packet_rate) / 10.0 << "M/s"
                << std::endl;

    for (Processor& p : processors)
    {
        std::cout
            << "Processor[" << (&p - processors.data()) << "].matches "
            << int(0.5 + 100.0 * p.getMatches() / headers.size()) << "%"
            << std::endl;
    }


}

int main()
{
    auto num_packets = 1000 * 1000;
    test(num_packets, 1);
    test(num_packets, 2);
    test(num_packets, 4);
    test(num_packets, 8);
}
