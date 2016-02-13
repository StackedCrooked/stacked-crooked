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


    for (auto i = 1ul; i <= 16u; ++i)
    {
        auto r = 0;//rand() % 2;
        Processor proc(src_ip, dst_ip, src_port + i, dst_port + i + r);
        processors.push_back(proc);
    }

    for (auto i = 0; i != 1000; ++i)
    {
        auto r1 = 0;//rand() % 2;
        auto r2 = 0;//rand() % 2;
        Header  header(src_ip, dst_ip, src_port + (i % processors.size()) + r1, dst_port + (i % processors.size()) + r2);
        headers.push_back(header);
    }



    auto total_counter = 0ul;
    for (auto i = 0; i != 10; ++i)
    {
        auto start_time = Clock::now();
        for (const Header& header : headers)
        {
            total_counter++;
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
            << " TotalOk=" << p.getOkCounted() << '/' << total_counter
            << " HashedOnly=" << p.getOkHashedOnly()
            << " ProcessedOnly=" << p.getOkProcessedOnly()
            << std::endl;
    }
}

int main()
{
    test_one_processor();
}
