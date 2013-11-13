#include "VTableImpl.h"

int VTableImpl::sum(int a, int b) const
{
    return a + b;
}

VTable* CreateVTable() { return new VTableImpl(); }
