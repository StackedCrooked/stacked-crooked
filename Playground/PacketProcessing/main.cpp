#include <iostream>
#include <vector>
#include <array>


struct Packet
{
};


struct Segment
{
    enum
    {
        max_packets = 65536,
        max_bytes   = max_packets * 64
    };

    std::array<uint16_t, max_packets> mPacketOffsets;
    std::array<uint8_t, max_bytes> mPacketData;
};

struct NetworkInterface
{
    Segment receiveSegment()
    {
        return Segment();
    }
};





int main()
{

}
