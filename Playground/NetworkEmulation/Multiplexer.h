#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H


#include "Networking.h"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>


struct Multiplexer;


struct MultiplexerPortListener
{
    virtual void on_receive(const uint8_t* data, uint32_t size) = 0;

    uint32_t mPortIndex = -1;
    Multiplexer* mMultiplexer = nullptr;
};


struct MultiplexerBackend
{
    virtual void on_receive(uint32_t port_index, const uint8_t* data, uint32_t size) = 0;

    void send_to_switch(uint32_t port_index, const uint8_t* data, uint32_t size);

    Multiplexer* mMultiplexer = nullptr;
};


struct Multiplexer
{
    Multiplexer();

    void attach_port(uint32_t port_index, MultiplexerPortListener& listener)
    {
        assert(!mPortListeners.at(port_index));
        mPortListeners[port_index] = &listener;
        listener.mPortIndex = port_index;
        listener.mMultiplexer = this;
    }

    void attach_backend(MultiplexerBackend& backend)
    {
        assert(!mBackend);
        mBackend = &backend;
        mBackend->mMultiplexer = this;
    }

    void receive_from_port(uint32_t port_index, const uint8_t* data, uint32_t size)
    {
        assert(port_index < mPortListeners.size());
        assert(mBackend);

        mBackend->on_receive(port_index, data, size);
    }

    void receive_from_backend(uint32_t port_index, const uint8_t* data, uint32_t size)
    {
        mPortListeners[port_index]->on_receive(data, size);
    }

    std::array<MultiplexerPortListener*, 48> mPortListeners;
    MultiplexerBackend* mBackend = nullptr;
};


#endif // MULTIPLEXER_H
