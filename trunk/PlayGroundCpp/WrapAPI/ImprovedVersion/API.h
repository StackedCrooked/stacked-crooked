#ifndef API_H
#define API_H


#include <string>
#include <vector>
#include <cstddef>


namespace API {


class Interface;
class ServerImpl;


class Server
{
public:
    static Server * Create();
    
    virtual ~Server();
    
    virtual Interface * createInterface();
    
protected:
    Server();
    
    ServerImpl * mImpl;
};


class Stream;


class Interface
{
public:
    static Interface * Create();
    
    virtual void addStream(Stream * stream);
    
    virtual const Stream * getStreamByIndex(std::size_t index) const;
    
    virtual Stream * getStreamByIndex(std::size_t index);
    
    virtual std::size_t streamCount() const;
    
protected:
    Interface();    
    std::vector<Stream*> mStreams;
};




class Stream
{
public:
    static Stream * Create();
    
    virtual ~Stream() = 0;
    
    virtual void start() = 0;
    
    virtual void stop() = 0;
    
protected:
    Stream();    
    std::string mStats;
};


} // namespace API


#endif // API_H
