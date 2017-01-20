#include "BBPort.h"
#include "BBInterface.h"
#include "PhysicalInterface.h"
#include "BBServer.h"
#include <algorithm>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>


namespace {


std::vector<uint8_t> cICMPRequest {
    0x00, 0x25, 0x90, 0x31, 0x82, 0x06, 0x00, 0x25,
    0x64, 0x9f, 0xda, 0x50, 0x08, 0x00, 0x45, 0x00,
    0x00, 0x54, 0x00, 0x00, 0x40, 0x00, 0x40, 0x01,
    0x21, 0xdd, 0x0a, 0x04, 0x01, 0x9e, 0x0a, 0x04,
    0x03, 0x27, 0x08, 0x00, 0x60, 0x3f, 0x1a, 0x61,
    0x00, 0x01, 0xb6, 0x7e, 0xc8, 0x4f, 0x00, 0x00,
    0x00, 0x00, 0x31, 0xbd, 0x0e, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
    0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d,
    0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
    0x36, 0x37,
};


std::vector<std::vector<uint8_t>> cICMPRequestBatch(32, cICMPRequest);


enum
{
    num_packets = 1000 * 1000 / 2
};


}



using SteadyClock = std::chrono::steady_clock;


std::chrono::nanoseconds run_test(BBServer& bbServer)
{
    auto start_time = SteadyClock::now();
    bbServer.run(cICMPRequestBatch, num_packets);
    auto elapsed_time = SteadyClock::now() - start_time;
    return elapsed_time;
}


int main()
{
    //*reinterpret_cast<MACAddress*>(cICMPRequestBatch.front().data()) = MACAddress::BroadcastAddress();

    BBServer bbServer;
    auto& bbPort = bbServer.mPhysicalInterface.getBBInterface(0).addPort(MACAddress{{ 0x00, 0x25, 0x90, 0x31, 0x82, 0x06}});

    std::array<std::chrono::nanoseconds, 64> tests;

    for (auto& ns : tests)
    {
        ns = run_test(bbServer);
    }

    std::sort(tests.begin(), tests.end());

    for (auto& ns : tests)
    {
        auto ns_per_packet = 1.0 * ns.count() / num_packets;
        auto budget_usage = int(0.5 + 100 * ns_per_packet / 25.0);
        auto Mpps = int(1.0 * num_packets / (ns.count() / 1e3));
        std::cout << "budget_consumed=" << budget_usage << "% Mpps=" << Mpps << " ns_per_packet=" << int(0.5 + ns_per_packet) << " cycles_per_packet=" << int(0.5 + 3.5 * ns_per_packet) << " UnicastCounter=" << bbPort.mUnicastCounter << " BroadcastCounter=" << bbPort.mBroadcastCounter << std::endl;
        break;
    }

    assert(bbPort.mUnicastCounter == num_packets * tests.size());
}
