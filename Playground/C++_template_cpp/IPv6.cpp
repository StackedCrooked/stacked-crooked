#include "IPv6.h"
#include "Port.h"


template<>
IPv6& Port::test<IPv6>()
{
    return *new IPv6;
}
