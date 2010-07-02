#include "RequestMethod.h"
#include <stdexcept>


namespace HSServer
{

    const char * ToString(RequestMethod inRequestMethod)
    {
        switch (inRequestMethod)
        {
            case RequestMethod_Get:
            {
                return "GET";
            }
            case RequestMethod_Post:
            {
                return "POST";
            }
            case RequestMethod_Put:
            {
                return "PUT";
            }
            case RequestMethod_Delete:
            {
                return "DELETE";
            }
            case RequestMethod_Unknown:
            default:
            {
                return "";
            }
        }
    }


    RequestMethodString::RequestMethodString(const std::string & inValue) :
        std::string(inValue)
    {
    }


    RequestMethod FromString(const RequestMethodString & inValue)
    {
        for (size_t idx = RequestMethod_Begin; idx != RequestMethod_End; ++idx)
        {
            RequestMethod requestMethod = static_cast<RequestMethod>(idx);
            if (inValue == ToString(requestMethod))
            {
                return requestMethod;
            }
        }
        return RequestMethod_Unknown;
    }

} // namespace HSServer
