#include "Stack.h"
#include <cstring>


Stack::Stack()
{
}

void Stack::add_to_queue(const RxPacket& packet)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mPackets.push_back(packet);
}


void Stack::flush()
{
    mPackets.clear();
}



