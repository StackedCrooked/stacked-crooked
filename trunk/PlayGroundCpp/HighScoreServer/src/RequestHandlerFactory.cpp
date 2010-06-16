#include "RequestHandlerFactory.h"
#include "RequestHandler.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/Application.h"


namespace HSServer
{
    
    RequestHandlerId GetRequestHandlerId(const Poco::Net::HTTPServerRequest & inRequest)
    {
        RequestMethod requestMethod = RequestMethod_Unknown;
        if (inRequest.getMethod() == "GET")
        {
            requestMethod = RequestMethod_Get;
        }
        else if (inRequest.getMethod() == "POST")
        {
            requestMethod = RequestMethod_Post;
        }

        if (requestMethod == RequestMethod_Unknown)
        {
            throw std::runtime_error("HTTP request method not supported: " + inRequest.getMethod());
        }
        
        // Get action url without parameters
        std::string action = inRequest.getURI().substr(0, inRequest.getURI().find("?"));

        return std::make_pair(requestMethod, action);
    }


    Poco::Net::HTTPRequestHandler *
    RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& inRequest)
    {
        // Log the request
        Poco::Util::Application::instance().logger().information("Request with uri: " + inRequest.getURI());

        FactoryFunctions::iterator it = mFactoryFunctions.find(GetRequestHandlerId(inRequest));
        if (it != mFactoryFunctions.end())
        {
            const FactoryFunction & ff(it->second);
            return ff(inRequest);
        }

        return new ErrorRequestHandler("No handler for " + inRequest.getURI());
    }

} // namespace HSServer
