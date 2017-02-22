#pragma once


#include "RxPacket.h"
#include <boost/lockfree/spsc_queue.hpp>
#include <vector>


struct Stack
{
    Stack();

    void add_to_queue(RxPacket rxPacket)
    {
        mPackets.push(rxPacket);
    }

    boost::lockfree::spsc_queue<RxPacket, boost::lockfree::capacity<1024>> mPackets;
};
