#ifndef REQUESTHANDLERID_H_INCLUDED
#define REQUESTHANDLERID_H_INCLUDED


#include "ResourceId.h"
#include "RequestMethod.h"
#include "ContentType.h"
#include <string>


namespace HSServer {

class RequestHandlerId
{
public:
    explicit RequestHandlerId(ResourceId inResourceId, Method inMethod, ContentType inContentType);

    inline ResourceId resourceId() const { return mResourceId; }

    inline ContentType preferredContentType() const { return mContentType; }

    inline Method requestMethod() const { return mMethod; }

private:
    ResourceId mResourceId;
    ContentType mContentType;
    Method mMethod;
};

bool operator < (const RequestHandlerId & lhs, const RequestHandlerId & rhs);

std::string ToString(const RequestHandlerId & inRequestHandlerId);


} // namespace HSServer


#endif // REQUESTHANDLERID_H_INCLUDED
