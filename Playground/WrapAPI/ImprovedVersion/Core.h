#ifndef CORE_H
#define CORE_H


#include "API.h"


namespace Core {


class Server : public API::Server
{
public:
    virtual ~Server();    
};


class Interface : public API::Interface
{
public:
    virtual ~Interface();
};


class Stream : public API::Stream
{
public:
    virtual ~Stream();
    
    virtual void start();
    
    virtual void stop();
};


} // Core


#endif // CORE_H
