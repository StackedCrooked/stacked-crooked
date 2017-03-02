#pragma once


#include "RxPacket.h"
#include <condition_variable>
#include <mutex>
#include <vector>


struct Stack
{
    Stack();
    Stack(const Stack&){}
    Stack& operator=(const Stack&){ return *this; }
    ~Stack() = default;

    void add_to_queue(const RxPacket& packet);

    void flush();

    std::mutex mMutex;
    std::vector<RxPacket> mPackets;
};
