#include "Switch.h"


Switch::Switch() :
    mPortListeners()
{
}


void SwitchPortListener::send_to_switch(const uint8_t* data, uint32_t size)
{
    assert(mSwitch);
    mSwitch->receive(mPortIndex, data, size);
}
