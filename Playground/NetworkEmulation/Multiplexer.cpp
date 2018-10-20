#include "Multiplexer.h"


Multiplexer::Multiplexer() :
    mPortListeners()
{
}


void MultiplexerBackend::send_to_switch(uint32_t port_index, const uint8_t* data, uint32_t size)
{
    mMultiplexer->receive_from_backend(port_index, data, size);
}
