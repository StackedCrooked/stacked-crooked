#include "BBServer.h"
#include "RxPacket.h"


BBServer::BBServer()
{
}


void BBServer::run(const std::vector<RxPacket>& rxPackets, uint32_t num_repeats)
{
    PhysicalInterface& physicalInterface = mPhysicalInterfaces[0];

    for (auto i = 0u; i != num_repeats; ++i)
    {
        const RxPacket* packets_ptr = rxPackets.data();
        uint32_t packets_len = rxPackets.size();

        while (packets_len >= 32)
        {
            physicalInterface.pop(packets_ptr, 32);
            packets_len -= 32;
            packets_ptr += 32;
        }

        if (packets_len > 0)
        {
            physicalInterface.pop(packets_ptr, packets_len);
        }
    }
}
