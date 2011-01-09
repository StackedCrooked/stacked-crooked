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
    explicit RequestHandlerId(ResourceId inResourceId, RequestMethod inRequestMethod);

    inline ResourceId resourceId() const { return mResourceId; }

    inline RequestMethod requestMethod() const { return mRequestMethod; }

private:
    ResourceId mResourceId;
    RequestMethod mRequestMethod;
};

bool operator < (const RequestHandlerId & lhs, const RequestHandlerId & rhs);

std::string ToString(const RequestHandlerId & inRequestHandlerId);


} // namespace HSServer


#endif // REQUESTHANDLERID_H_INCLUDED
