#include "RequestMethod.h"
#include <stdexcept>


namespace HSServer
{

    const std::string & ToString(RequestMethod inRequestMethod)
    {
        switch (inRequestMethod)
        {
            case RequestMethod_Get:
            {
                static std::string sResult = "GET";
                return sResult;
            }
            case RequestMethod_Post:
            {
                static std::string sResult = "POST";
                return sResult;
            }
            case RequestMethod_Put:
            {
                static std::string sResult = "PUT";
                return sResult;
            }
            case RequestMethod_Delete:
            {
                static std::string sResult = "DELETE";
                return sResult;
            }
            case RequestMethod_Unknown:
            {
                static std::string sResult = "RequestMethod_Unknown";
                return sResult;
            }
            default:
            {
                throw std::logic_error("ToString(RequestMethod): invalid enum value of type RequestMethod.");
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
