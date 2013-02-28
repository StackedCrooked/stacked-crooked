#pragma once


#include <memory>
#include <stdint.h>


class MessageServer
{
public:
    MessageServer(uint16_t port);
    
    
    
private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;
};
