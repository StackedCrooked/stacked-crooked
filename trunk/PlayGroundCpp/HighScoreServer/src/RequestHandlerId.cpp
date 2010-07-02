#include "RequestHandlerId.h"
#include "Poco/Format.h"
#include "Poco/String.h"
#include <stdio.h>


namespace HSServer
{
    
    RequestHandlerId::RequestHandlerId(const std::string & inLocation,
                                       RequestMethod inRequestMethod,
                                       ContentType inContentType) :
        mLocation(inLocation),
        mRequestMethod(inRequestMethod),
        mContentType(inContentType)
    {
    }

    const std::string & RequestHandlerId::location() const
    {
        return mLocation;
    }

    RequestMethod RequestHandlerId::requestMethod() const
    {
        return mRequestMethod;
    }

    ContentType RequestHandlerId::contentType() const
    {
        return mContentType;
    }

    bool operator < (const RequestHandlerId & lhs, const RequestHandlerId & rhs)
    {
        if (lhs.requestMethod() != rhs.requestMethod())
        {
            return lhs.requestMethod() < rhs.requestMethod();
        }
        else if (lhs.contentType() != rhs.contentType())
        {
            return lhs.contentType() < rhs.contentType();
        }
        else
        {
            return lhs.location() < rhs.location();
        }
    }
    
    
    std::string ToString(const RequestHandlerId & inRequestHandlerId)
    {
        return Poco::format("{location: %s; requestMethod: %s; contentType: %s }",
                            inRequestHandlerId.location(),
                            std::string(ToString(inRequestHandlerId.requestMethod())),
                            std::string(ToString(inRequestHandlerId.contentType())));
    }

} // namespace HSServer
