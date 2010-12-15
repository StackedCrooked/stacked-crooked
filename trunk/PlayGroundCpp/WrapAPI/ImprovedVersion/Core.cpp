#include "Core.h"


namespace Core {


API::Interface * Server::createInterface()
{
    return new Core::Interface();
}


void Interface::addStream(API::Stream * stream)
{
    mStreams.push_back(stream);
}


const API::Stream * Interface::getStreamByIndex(std::size_t index) const
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
