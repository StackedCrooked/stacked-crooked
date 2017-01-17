#include "Range.h"
#include <vector>
#include <forward_list>
#include <iostream>



struct FlowCounters
{
    uint64_t mRxPackets;
    uint64_t mRxBytes;
};



struct BPFFilter
{

};


struct UDPFlow
{

    bool match(Packet packet) const
    {

    }

    uint64_t mMask[2];
    uint64_t mRxPackets;
    uint64_t mRxBytes;
};


struct Trigger
{

};


struct UDPFlow
{

    bool match(Packet p, uint32_t l3offset) const
    {
        auto data = p.data() + l3offset + mask_offset;

        auto u64 = Decode<std::array<uint64_t, 2>>(data);

        return ((u64[0] & mFields[0]) == mMasks[0])
            && ((u64[1] & mFields[1]) == mMasks[1]);
    }


    void pop(Packet p, uint32_t l3offset)
    {
        if (match(p, l3offset))
        {
            mCounters.mRxBytes += p.size();
            mCounters.mRxPackets++;
        }
    }

    uint64_t mMasks[2];
    FlowCounters mCounters;
};


struct UDPFlows
{
    void pop(Packet packet, uint32_t layer3_offset)
    {
        auto hash = get_hash(packet);
        mHashTable[hash];
    }

    HashTable mHashTable;
    std::vector<FlowCounters> mFlows;
};



struct BBPort
{
    void pop(Packet packet)
    {
        // validate mac address and vlan tags
        if (!validate(packet))
        {
            return;
        }
    }

    std::vector<UDPFlow> mUDPFlows;
    uint32_t mLayer3Offset;
    uint64_t mUnicastCount;
    uint64_t mBroadcastCount;
};


struct BBInterface
{
    void pop(Packet packet)
    {
        for (BPFFilter& bpf : mBPFFilters)
        {
            bpf.pop(packet);
        }

        for (BBPort& bbPort : mBBPorts)
        {
            bbPort.pop(packet);
        }
    }

    std::vector<BPFFilter> mBPFFilters;
    std::vector<BBPort> mBBPorts;
};


struct PhysicalInterface
{
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



