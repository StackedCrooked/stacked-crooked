#pragma once


#include "RxPacket.h"


struct Stack
{
    Stack();

    void pop(RxPacket packet);
};
