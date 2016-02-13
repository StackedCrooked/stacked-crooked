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
