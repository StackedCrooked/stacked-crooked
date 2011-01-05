#ifndef REQUESTHANDLERID_H_INCLUDED
#define REQUESTHANDLERID_H_INCLUDED


#include "RequestMethod.h"
#include "ContentType.h"
#include <string>


namespace HSServer
{

    class RequestHandlerId
    {
    public:
        explicit RequestHandlerId(const std::string & inLocation, RequestMethod inRequestMethod);

        const std::string & location() const;

        RequestMethod requestMethod() const;

    private:
        std::string mLocation;
        RequestMethod mRequestMethod;
    };

    bool operator < (const RequestHandlerId & lhs, const RequestHandlerId & rhs);

    std::string ToString(const RequestHandlerId & inRequestHandlerId);

} // namespace HSServer


#endif // REQUESTHANDLERID_H_INCLUDED
