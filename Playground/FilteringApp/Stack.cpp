#include "Stack.h"


Stack::Stack()
{
    mFreeBuffers.resize(128);
    for (PacketBuffer*& buf : mFreeBuffers)
    {
        buf = new PacketBuffer();
    }
}


void Stack::add_to_queue(RxPacket packet)
{
    if (mFreeBuffers.empty())
    {
        throw "NO";
    }

    PacketBuffer* buffer = mFreeBuffers.back();
    mFreeBuffers.pop_back();

    memcpy(buffer->mBuffer, packet.data(), packet.size());

    mPackets.push_back(buffer);

    if (mPackets.size() == 128)
    {
        flush();
    }
}


void Stack::flush()
{
    mFreeBuffers.insert(mFreeBuffers.end(), mPackets.begin(), mPackets.end());
    mPackets.clear();
}



