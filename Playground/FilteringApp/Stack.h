#pragma once


#include "RxPacket.h"


struct Stack
{
    void add_to_queue(RxPacket packet);
};
