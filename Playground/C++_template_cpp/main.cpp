#include "Port.h"
#include "IPv4.h"
#include "IPv6.h"


int main()
{
    Port port;
    port.test<IPv4>();
    port.test<IPv6>();
}
    

