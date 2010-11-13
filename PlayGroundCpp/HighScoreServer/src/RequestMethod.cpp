#include "RequestMethod.h"
#include <stdexcept>


namespace HSServer {


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


} // namespace HSServer
