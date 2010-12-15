
#include "API.h"
#include "Core.h"


namespace API {


APIObject::APIObject(Core::CoreObject * inCoreObject) :
    mCoreObject(inCoreObject)
{
}

APIObject::~APIObject()
{
}

Core::CoreObject * APIObject::getCoreObject()
{
    return mCoreObject;
}


const Core::CoreObject * APIObject::getCoreObject() const
{
    return mCoreObject;
}


void APIObject::setCoreObject(Core::CoreObject * inCoreObject)
{
    mCoreObject = inCoreObject;
}


//
// APIServer
//
APIServer::APIServer() :
    APIObject(new Core::Server)
{
}


APIInterface * APIServer::createInterface()
{
    Core::Server * coreServer = static_cast<Core::Server*>(getCoreObject());
    Core::Interface * coreInterface = coreServer->createInterface();
    APIInterface * result(new API::APIInterface);
    result->setCoreObject(coreInterface);
    return result;
}
    



//
// APIInterface
//
APIInterface::APIInterface() :
    APIObject(new Core::Interface)
{
}


void APIInterface::addStream(APIStream * apiStream)
{
    Core::Stream * coreStream = static_cast<Core::Stream *>(apiStream->getCoreObject());
    Core::Interface * coreInterface = static_cast<Core::Interface*>(getCoreObject());
    coreInterface->addStream(coreStream);
}
   

//
// APIStream
//
const APIStream * APIInterface::getStreamByIndex(std::size_t index) const
{
    const Core::Interface * coreInterface = static_cast<const Core::Interface*>(getCoreObject());
    const Core::Stream * coreStream = coreInterface->getStreamByIndex(index);
    
    // Now how I get the the APIStream object?
    return 0;
}

APIStream * APIInterface::getStreamByIndex(std::size_t index)
{
    return const_cast<APIStream*>(static_cast<const APIInterface*>(this)->getStreamByIndex(index));
}
    
std::size_t APIInterface::streamCount() const
{
    const Core::Interface * coreInterface = static_cast<const Core::Interface*>(getCoreObject());
    return coreInterface->streamCount();
}


APIStream::APIStream() :
    APIObject(new Core::Stream)
{
}


void APIStream::start()
{
    static_cast<Core::Stream*>(getCoreObject())->start();
}
   
     
void APIStream::stop()
{
    static_cast<Core::Stream*>(getCoreObject())->stop();
}


} // API
