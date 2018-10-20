#include "PCAPWriter.h"


uint8_t buf[1514]; // standard ethernet size backet

int main()
{
    PCAPWriter pcap("pcap.pcap");
    for (auto i = 0;  i != 1e6; ++i)
    {
        pcap.push_back(buf, sizeof(buf));
    }
}
