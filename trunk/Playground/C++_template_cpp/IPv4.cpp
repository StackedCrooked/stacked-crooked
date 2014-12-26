#include "Port.h"
#include "IPv4.h"


template<>
IPv4& Port::test<IPv4>()
{
    return *new IPv4;
}
