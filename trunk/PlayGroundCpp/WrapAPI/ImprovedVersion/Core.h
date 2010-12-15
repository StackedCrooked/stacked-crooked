#ifndef CORE_H
#define CORE_H


#include "API.h"


namespace Core {


class Server : public API::Server
{
public:
    virtual API::Interface * createInterface();
    
};


class Interface : public API::Interface
{
public:
    virtual void addStream(API::Stream * stream);
    
    virtual const API::Stream * getStreamByIndex(std::size_t index) const;
    
    virtual std::size_t streamCount() const;
};


class Stream : public API::Stream
{
public:
    virtual void start();
    
    virtual void stop();
};


} // Core


#endif // CORE_H
