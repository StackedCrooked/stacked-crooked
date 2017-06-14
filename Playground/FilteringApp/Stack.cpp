#include "Stack.h"
#include <cstring>


Stack::Stack()
{
    mFreeBuffers.resize(128);
    for (PacketBuffer*& buf : mFreeBuffers)
    {
        buf = new PacketBuffer();
    }
}


void Stack::add_to_queue(RxPacket)
{
    // TODO
}


void Stack::flush()
{
    mFreeBuffers.insert(mFreeBuffers.end(), mPackets.begin(), mPackets.end());
    mPackets.clear();
}



