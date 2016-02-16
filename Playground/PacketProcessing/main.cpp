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




void test_one_processor()
{
    std::cout << "DETECTED FREQ=" << get_frequency_mhz() << "Mhz\n";
    std::vector<Header> headers;
    headers.reserve(1000 * 1000);

    std::vector<Processor> processors;
    processors.reserve(2);

    uint16_t src_port = 1024;
    uint16_t dst_port = 1024;


    for (auto i = 0ul; i < processors.capacity(); ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1 + i / 2);
        IPv4Address dst_ip(1, 1, 2, 1 + i / 2);
        auto sp = src_port + i % 4;
        auto dp = dst_port + i % 4;
        Processor proc(src_ip, dst_ip, sp, dp);
		std::cout << "Proc: " << src_ip << ' ' << dst_ip << ' ' << sp << ' ' << dp << '\n';
        processors.push_back(proc);
    }

    for (auto i = 0ul; i < headers.capacity(); ++i)
    {
        IPv4Address src_ip(1, 1, 1, 1 + i % processors.size());
        IPv4Address dst_ip(1, 1, 2, 1 + i % processors.size());
		auto sp = src_port + i % 4;
		auto dp = dst_port + i % 4;
        if (i < 2 * processors.capacity())
        {
            std::cout << src_ip << ' ' << dst_ip << ' ' << sp << ' ' << dp << '\n';
        }
        Header header(src_ip, dst_ip, sp, dp);
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


    for (Processor& p : processors)
    {
        auto expected_count = headers.size() / processors.size();
        std::cout
            << "Processor-index=" << (&p - processors.data())
            << " Matched=" << int(0.5 + 100.0 * p.getMatches() / expected_count) << "%"
            << std::endl;
    }

        std::cout << "packets=" << total_counter << " cycles=" << elapsed_time << " ns=" << elapsed_time / get_frequency_ghz()
            << "\n cycles/packet=" << (1.0 * elapsed_time / headers.size()) << " (" << (1.0 * elapsed_time / get_frequency_ghz() / headers.size()) << "ns)"
            << "\n cycles/packet/filter=" << (1.0 * elapsed_time / headers.size() / processors.size()) << " (" << (1.0 * elapsed_time / headers.size() / processors.size()) / get_frequency_ghz() << "ns)"
            << '\n';

}

int main()
{
    test_one_processor();
}
