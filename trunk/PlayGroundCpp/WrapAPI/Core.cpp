#include "Core.h"


namespace Core {


CoreObject::~CoreObject()
{
}


Interface * Server::createInterface()
{
    mInterfaces.push_back(new Interface);
    return mInterfaces.back();
}


void Interface::addStream(Stream * stream)
{
    mStreams.push_back(stream);
}


const Stream * Interface::getStreamByIndex(std::size_t index) const
{
    return mStreams.at(index);
}


std::size_t Interface::streamCount() const
{
    return mStreams.size();
}


void Stream::start()
{
     // Implementation ...
}
 
       
void Stream::stop()
{
     // Implementation ...
}


} // Core
