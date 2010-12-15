#ifndef API_H
#define API_H


#include <string>
#include <vector>


namespace Core {
class CoreObject;
}


namespace API {


class APIStream;
class APIInterface;


// Base class for all API classes.
class APIObject
{
public:
    APIObject(Core::CoreObject * inCoreObject);
    
    virtual ~APIObject();
    
    Core::CoreObject * getCoreObject();
    
    const Core::CoreObject * getCoreObject() const;
    
    void setCoreObject(Core::CoreObject * inCoreObject);
    
private:
    Core::CoreObject * mCoreObject; 
};


class APIServer : public APIObject
{
public:
    APIServer();
    
    APIInterface * createInterface();
};


class APIInterface : public APIObject
{
public:
    APIInterface();
    
    void addStream(APIStream * stream);
    
    const APIStream * getStreamByIndex(std::size_t index) const;
    
    APIStream * getStreamByIndex(std::size_t index);
    
    std::size_t streamCount() const;
};


class APIStream : public APIObject
{
public:
    APIStream();
    
    void start();
    
    void stop();
};


} // API

#endif // API_H
