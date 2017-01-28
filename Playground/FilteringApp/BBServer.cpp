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
        for (const RxPacket& rxPacket : rxPackets)
        {
            physicalInterface.pop(rxPacket);
        }
    }
}


