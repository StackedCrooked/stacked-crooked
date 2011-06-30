#include "RequestHandlerId.h"
#include "Poco/Format.h"
#include "Poco/String.h"
#include <stdio.h>


namespace HSServer {


RequestHandlerId::RequestHandlerId(ResourceId inResourceId,
                                    Method inMethod,
                                    ContentType inContentType) :
    mResourceId(inResourceId),
    mMethod(inMethod),
    mContentType(inContentType)
{
}


bool operator< (const RequestHandlerId & lhs, const RequestHandlerId & rhs)
{
    if (lhs.resourceId() != rhs.resourceId())
    {
        return lhs.resourceId() < rhs.resourceId();
    }
    else if (lhs.preferredContentType() != rhs.preferredContentType())
    {
        return lhs.preferredContentType() < rhs.preferredContentType();
    }
    else
    {
        return lhs.requestMethod() < rhs.requestMethod();
    }
}


std::string ToString(const RequestHandlerId & inRequestHandlerId)
{
    return Poco::format("{ location: %s; requestMethod: %s ; contentType: %s}",
                        ResourceManager::Instance().getResourceLocation(inRequestHandlerId.resourceId()),
                        std::string(ToString(inRequestHandlerId.requestMethod())),
                        std::string(ToString(inRequestHandlerId.preferredContentType()))
                        );
}


} // namespace HSServer
