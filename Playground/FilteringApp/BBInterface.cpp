#include "BBInterface.h"










void BBInterface::pop_now()
{
    auto len = mBatchSize;
    auto data = mBatch.data();

    auto& bbPort = mBBPorts.front();

    while (len >= 8)
    {
        bbPort.pop(data[0].first, data[0].second);
        bbPort.pop(data[1].first, data[1].second);
        bbPort.pop(data[2].first, data[2].second);
        bbPort.pop(data[3].first, data[3].second);
        bbPort.pop(data[4].first, data[4].second);
        bbPort.pop(data[5].first, data[5].second);
        bbPort.pop(data[6].first, data[6].second);
        bbPort.pop(data[7].first, data[7].second);
        data += 8;
        len -= 8;
    }
    assert(len == 0);
    mBatchSize = 0;
}
