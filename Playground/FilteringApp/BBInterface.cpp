#include "BBInterface.h"


BBInterface::BBInterface()
{
    mPacketBatch.reserve(32);
}


BBPort& BBInterface::addPort(MACAddress localMAC)
{
    mBBPorts.push_back(std::make_unique<BBPort>(localMAC));
    return *mBBPorts.back();
}


void BBInterface::pop_now()
{
    for (std::unique_ptr<BBPort>& port : mBBPorts)
    {
        BBPort& bbPort = *port;
        bbPort.pop_many(mPacketBatch.data(), mPacketBatch.size());
    }

    for (RxTrigger& rxTrigger : mRxTriggers)
    {
        rxTrigger.process(mPacketBatch);
    }

    mPacketBatch.clear();
}
