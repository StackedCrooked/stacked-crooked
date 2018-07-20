#ifndef SWITCH_H
#define SWITCH_H


#include "Networking.h"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>
#include <vector>


/**

TODO
----
- mPortIndex is zero-based, but that's not clear
- Broadcast messages
- Cubro switch
- Backend device


 */


struct Switch;


struct SwitchPortListener
{
    virtual void on_receive(const uint8_t* data, uint32_t size) = 0;

    void send_to_switch(const uint8_t* data, uint32_t size);

    Switch* mSwitch = nullptr;
    uint32_t mPortIndex = -1;
};


struct Switch
{
    Switch();

    void attach(uint32_t port_index, SwitchPortListener& listener)
    {
        assert(!mPortListeners.at(port_index));
        mPortListeners.at(port_index) = &listener;
        listener.mPortIndex = port_index;
        listener.mSwitch = this;
    }

    void receive(uint32_t port_index, const uint8_t* data, uint32_t size)
    {
        assert(port_index < mPortListeners.size());


        auto dst_mac = Decode<MACAddress>(data);
        assert(dst_mac != MACAddress(0));

        // Update dynamic config
        addDestinationPort(port_index, Decode<MACAddress>(data + sizeof(MACAddress)));

        if (SwitchPortListener* port_listener = findDestinationPort(dst_mac))
        {
            port_listener->on_receive(data, size);
            return;
        }

        // FLOOD
        std::cout << "*** Switch doesn't know where " << dst_mac << " is => sending message to all ports!" << std::endl;
        for (SwitchPortListener* port : mPortListeners)
        {
            if (port)
            {
                port->on_receive(data, size);
            }
        }
    }

    void addDestinationPort(uint32_t port_index, const MACAddress& mac)
    {
        assert(mac != MACAddress(0));

        if (mDynamicConfig.count(mac))
        {
            return;
        }

        std::cout << "Switch learns that " << mac << " is behind port " << port_index << std::endl;
        mDynamicConfig[mac] = port_index;
    }

    SwitchPortListener* findDestinationPort(const MACAddress& dst_mac)
    {
        auto it = mDynamicConfig.find(dst_mac);
        if (it == mDynamicConfig.end())
        {
            return nullptr;
        }

        return mPortListeners[it->second];
    }

    std::map<MACAddress, uint32_t> mDynamicConfig;
    std::array<SwitchPortListener*, 48> mPortListeners;
};


#endif // SWITCH_H
