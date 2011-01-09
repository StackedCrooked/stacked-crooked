#include "RequestHandlerId.h"
#include "Poco/Format.h"
#include "Poco/String.h"
#include <stdio.h>


namespace HSServer {


RequestHandlerId::RequestHandlerId(ResourceId inResourceId,
                                    RequestMethod inRequestMethod) :
    mResourceId(inResourceId),
    mRequestMethod(inRequestMethod)
{
}


bool operator < (const RequestHandlerId & lhs, const RequestHandlerId & rhs)
{
    if (lhs.requestMethod() != rhs.requestMethod())
    {
        return lhs.requestMethod() < rhs.requestMethod();
    }
    else
    {
        return lhs.resourceId() < rhs.resourceId();
    }
}
    
    
std::string ToString(const RequestHandlerId & inRequestHandlerId)
{
    return Poco::format("{ location: %s; requestMethod: %s }",
                        std::string(cResourceLocations[inRequestHandlerId.resourceId()]),
                        std::string(ToString(inRequestHandlerId.requestMethod())));
}


} // namespace HSServer
