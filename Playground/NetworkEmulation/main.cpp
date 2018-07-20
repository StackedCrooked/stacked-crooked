#include "Switch.h"
#include <iostream>


struct Message
{
    Message() = default;

    Message(MACAddress dst, MACAddress src, uint16_t ethertype):
        mDestination(dst),
        mSource(src),
        mEtherType(ethertype)
    {
    }

    const uint8_t* data() const { return mDestination.data(); }
    uint32_t size() const { return sizeof(*this); }

    MACAddress mDestination;
    MACAddress mSource;
    uint16_t mEtherType = 0;
};


struct Device : SwitchPortListener
{
    explicit Device(const MACAddress& mac) :
        mMAC(mac)
    {
    }

    void on_receive(const uint8_t* data, uint32_t /*size*/) override final
    {
        auto dst_mac = Decode<MACAddress>(data);
        if (dst_mac == mMAC)
        {
            std::cout << "" << mMAC << " successfully received packet" << std::endl;
        }
        else
        {
            std::cout << "*** " << mMAC << " received packet that was intended for " << dst_mac << std::endl;

        }
    }

    void send(const Device& target)
    {
        assert(this != &target); // TODO: FR: for now
        assert(mMAC != target.mMAC); // TODO: FR: for now

        Message msg(target.mMAC, mMAC, 0);

        std::cout << "" << mMAC << " sends message to " << target.mMAC << std::endl;
        send_to_switch(msg.data(), msg.size());
    }


    MACAddress mMAC;
};


int main()
{
    Device d1(MACAddress(1));
    Device d2(MACAddress(2));
    Device d3(MACAddress(3));

    Switch smc;
    smc.attach(1, d1);
    smc.attach(2, d2);

    std::cout << "=== Step 1 ===" << std::endl;
    d1.send(d2);
    d2.send(d1);

    std::cout << "\n=== Step 2 ===" << std::endl;
    d1.send(d2);
    d2.send(d1);

    std::cout << "\n=== Step 3 ===" << std::endl;
    d1.send(d3);
}
