#include "Processor.h"
#include <array>
#include <algorithm>
#include <boost/container/static_vector.hpp>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <cassert>
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

void test_one_processor()
{
    std::vector<Header> headers;

    std::vector<Processor> processors;

    IPv4Address src_ip(1, 1, 1, 1);
    IPv4Address dst_ip(1, 1, 1, 1);
    uint16_t src_port = 1024;
    uint16_t dst_port = 1024;


    for (auto i = 1ul; i <= 8u; ++i)
    {
        Header  header(src_ip, dst_ip, src_port + (i / 2), dst_port);
        Processor proc(src_ip, dst_ip, src_port + (i / 2), dst_port);
        //assert(header.calculate_hash() == proc.hash());
        headers.push_back(header);
        processors.push_back(proc);
    }

    auto copy = headers;

    for (auto i = 0; i != 10; ++i)
    {
        headers.insert(headers.end(), copy.begin(), copy.end());
    }


    for (auto i = 0; i != 10; ++i)
    {
        auto start_time = Clock::now();
        for (const Header& header : headers)
        {
            auto hash = header.calculate_hash();
            for (Processor& processor : processors)
            {
                processor.process(hash, header.data(), header.size());
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
            << p.mProcessed << '/' << headers.size()
            << " HashesOk=" << p.mHashesOk << '/' << headers.size()
            << " " << int(0.5 + 100.0 * p.mProcessed / headers.size()) << "%\n";
    }
}

int main()
{
    test_one_processor();
}
