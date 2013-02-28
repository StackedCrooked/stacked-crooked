#pragma once


#include <memory>
#include <stdint.h>


class TCPServer
{
public:
    TCPServer(uint16_t port);
    
    
    
private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;
};
