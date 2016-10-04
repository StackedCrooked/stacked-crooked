#include <string>
#include <stdexcept>
#include <iostream>
#include "pcap.h"


struct BPFFilter
{
    BPFFilter(const std::string& bpf_filter)
    {
        using DummyInterface = std::unique_ptr<pcap_t, decltype(&pcap_close)>;

        DummyInterface dummy_interface(pcap_open_dead(DLT_EN10MB, 1518), &pcap_close);

        if (!dummy_interface)
        {
            throw std::runtime_error("Failed to open pcap dummy interface");
        }

        auto result = pcap_compile(dummy_interface.get(), &mProgram, bpf_filter.c_str(), 1, 0xff000000);
        if (result != 0)
        {
            std::cout << "pcap_geterr: [" << pcap_geterr(dummy_interface.get()) << "]" << std::endl;
            throw std::runtime_error("pcap_compile failed. Filter=" + bpf_filter);
        }
    }

    ~BPFFilter()
    {
        pcap_freecode(&mProgram);
    }

    static void check_slow(const char* filter, const uint8_t* data, uint32_t length)
    {
        std::cout << filter << ": " << BPFFilter(filter).check(data, length) << std::endl;
    }

    bool check(const uint8_t* data, uint32_t length)
    {
        return bpf_filter(mProgram.bf_insns, const_cast<uint8_t*>(data), length, length);
    }

    bpf_program mProgram;
};



const uint8_t tcp_syn[] = {
    0x00, 0x00, 0x00, 0x00,   0x00, 0x22, 0x00, 0x01,
    0x5c, 0x6c, 0xc2, 0x02,   0x08, 0x00, 0x45, 0x00,
    0x00, 0x34, 0x2b, 0x73,   0x00, 0x00, 0x3e, 0x06,
    0x3b, 0x3b, 0x0a, 0x0a,   0x01, 0x02, 0x0a, 0x0a,
    0x01, 0x01, 0xe0, 0x89,   0xec, 0xc6, 0x11, 0x51,
    0xd9, 0x98, 0x00, 0x00,   0x00, 0x00, 0x80, 0x02,
    0xff, 0xff, 0x9c, 0xc6,   0x00, 0x00, 0x02, 0x04,
    0x05, 0xb4, 0x03, 0x03,   0x05, 0x01, 0x04, 0x02,
    0x01, 0x01
};


using Clock = std::chrono::steady_clock;


int main()
{
    // src 10.10.1.2 dst 10.10.1.1
    // 60614 57481
    BPFFilter filter1("len == 66 && ether src 00:01:5c:6c:c2:02 && ether dst 00:00:00:00:00:22 && ip src 10.10.1.2 && ip dst 10.10.1.1 && tcp src port 57481 && tcp dst port 60614");
    BPFFilter filter2("len == 66 && ether src 00:01:5c:6c:c2:02 && ether dst 00:00:00:00:00:22 && ip src 10.10.1.2 && ip dst 10.10.1.1 && tcp src port 57481 && tcp dst port 60614");

    enum { num_iterations = 1000 };

        auto start_time = Clock::now();
        for (auto i = 0; i != num_iterations / 2; ++i)
        {
            filter1.check(tcp_syn, sizeof(tcp_syn));
            filter2.check(tcp_syn, sizeof(tcp_syn));
        }

        std::chrono::nanoseconds elapsed_ns = Clock::now() - start_time;
        std::cout
            << "elapsed_ns=" << elapsed_ns.count()
            << " ns/check=" << elapsed_ns.count()/num_iterations
            << " result=" << filter1.check(tcp_syn, sizeof(tcp_syn))
            << " size=" << sizeof(tcp_syn)
            << '\n';

}
