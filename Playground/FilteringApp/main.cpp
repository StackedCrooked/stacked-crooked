#include "Range.h"
#include <vector>
#include <iostream>


struct RxPacket
{
    const uint8_t* data() const;
    uint32_t size() const;
};



struct BBPort
{
    void receive(RxPacket /*packet*/)
    {
    }
};


struct BBInterface
{
    void receive(RxPacket packet)
    {
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



