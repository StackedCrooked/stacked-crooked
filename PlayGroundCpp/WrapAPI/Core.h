#ifndef CORE_H
#define CORE_H


#include <string>
#include <vector>


namespace Core {


// Base class for all Core classes.
class CoreObject
{
public:
    virtual ~CoreObject();
};


class Interface;
class Stream;


class Server : public CoreObject
{
public:
    Interface * createInterface();
    
private:
    std::vector<Interface*> mInterfaces;
    
};


class Interface : public CoreObject
{
public:
    void addStream(Stream * stream);
    
    const Stream * getStreamByIndex(std::size_t index) const;
    
    std::size_t streamCount() const;
    
private:
    std::vector<Stream*> mStreams;
};


class Stream : public CoreObject
{
public:
    void start();
    
    void stop();
    
private:
    std::string mStats;
};


} // Core



#endif // CORE_H
