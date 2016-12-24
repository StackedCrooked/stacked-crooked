#include "PCAPWriter.h"


uint8_t buf[1000 * 1000];

int main()
{
	PCAPWriter pcap("pcap.pcap");
	for (auto i = 0;  i != 10 * 1000; ++i)
	{
		pcap.push_back(buf, sizeof(buf));
	}
}
