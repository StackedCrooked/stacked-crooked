#include "API.h"
#include "Core.h"


namespace API {




Server::Server()
{
}


Server::~Server()
{
}


Server * Server::Create()
{
    return new Core::Server;
}


Interface * Server::createInterface()
{
    mInterfaces.push_back(Interface::Create());
    return mInterfaces.back();
}
    

Interface * Interface::Create()
{
    return new Core::Interface();
}
    

Interface::Interface()
{
}


void Interface::addStream(Stream * stream)
{
    mStreams.push_back(stream);
}
   

const Stream * Interface::getStreamByIndex(std::size_t index) const
{
    return mStreams.at(index);
}


Stream * Interface::getStreamByIndex(std::size_t index)
{
    return mStreams.at(index);
}

    
std::size_t Interface::streamCount() const
{
    return mStreams.size();
}


Stream * Stream::Create()
{
    return new Core::Stream;
}


Stream::Stream()
{
}


Stream::~Stream()
{
}


} // namespace API
