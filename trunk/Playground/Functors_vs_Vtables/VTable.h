#pragma once

struct VTable
{
    VTable();
    virtual int sum(int a, int b) const = 0;
};

VTable* CreateVTable();
