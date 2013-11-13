#pragma once
#include "VTable.h"

struct VTableImpl : VTable
{
    virtual int sum(int a, int b) const;
};
