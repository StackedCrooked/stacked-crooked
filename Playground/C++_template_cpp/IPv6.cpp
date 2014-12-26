#include "IPv6.h"
#include "Port.h"


template<>
IPv6& Port::test<IPv6>()
{
    printf("private int n=%d\n", n);
    return *new IPv6;
}
