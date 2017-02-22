#include "Stack.h"
#include <cstring>


Stack::Stack() :
    mConsumerThread([this]{ run_consumer(); })
{
}


Stack::~Stack()
{
    stop();
    //std::cout << this << " Stack::mRxPackets=" << mRxPackets << std::endl;
}


void Stack::stop()
{
    mPackets.push(RxPacket(nullptr, 0, 0));
    mConsumerThread.join();
}


void Stack::run_consumer()
{
    std::array<RxPacket, 32> batch;
    for (;;)
    {
        auto n = mPackets.pop(batch.data(), batch.size());
        if (n == 0)
        {
            asm volatile ("pause;");
            continue;
        }

        for (auto i = 0ul; i != n; ++i)
        {
            RxPacket rxPacket = batch[i];
            if (!rxPacket.data())
            {
                return;
            }
        }

        mRxPackets += n;
    }
}
