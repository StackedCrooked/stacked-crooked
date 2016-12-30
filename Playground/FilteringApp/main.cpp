#include "Range.h"
#include <vector>
#include <iostream>


struct RxPacket
{
    const uint8_t* data() const;
    uint32_t size() const;
};


using RxPackets = Range<RxPacket>;



struct Processor_Flow
{
    void receive(RxPackets packet)
    {
        auto l3 = packet.getLayer3Offset();
        (void)l3;
        //etc...
    }

    uint64_t mFields[2];
    uint64_t mMasks[2];
};


struct Processor_BPF
{
    void receive(RxPacket packet)
    {
        // bpf_filter(...)
    }
};



struct BBPort
{
    void receive(RxPacket /*packet*/)
    {
    }
};


struct BBInterface
{
    void receive(RxPackets packets)
    {
        //std::stable_sort(packets.begin(), packets.end(), [](Packet p) { return p.
        BBPort& bbPort = get_bb_port(packet);
        bbPort.receive(packet);
    }

    BBPort& get_bb_port(RxPacket);
};


struct PhysicalInterface
{
    // Receive packet from socket
    // - Dispatch to flow-based counters (5-tuple based)
    // - Dispatch to special counters (multicast and broadcast counters)
    // - Dispatch to bpf-based processors (user provided a BPF string)
    // - Dispatch to stack
    void receive(RxPacket packet)
    {
        BBInterface& bbInterface = get_bb_interface(packet);
        bbInterface.receive(packet);
    }

    BBInterface& get_bb_interface(RxPacket);

private:
    std::vector<BBInterface> mBBInterfaces;
};


struct BBServer
{
    void run()
    {
    }

};


int main()
{
    std::cout << "Hello!" << std::endl;

    BBServer bbServer;
    bbServer.run();

    std::cout << "Goodbye!" << std::endl;
}



