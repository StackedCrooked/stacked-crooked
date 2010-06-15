#include "RequestHandler.h"
#include "RequestHandlerFactory.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/Application.h"


namespace HSServer
{

    Poco::Net::HTTPRequestHandler *
    RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& inRequest)
    {
        // Log the request
        Poco::Util::Application::instance().logger().information("Request with uri: " + inRequest.getURI());

        FactoryFunctions::iterator it = mFactoryFunctions.find(inRequest.getURI().substr(0, inRequest.getURI().find("&")));
        if (it != mFactoryFunctions.end())
        {
            const FactoryFunction & ff(it->second);
            return ff(inRequest);
        }

        return new ErrorRequestHandler("No handler for " + inRequest.getURI());
    }

} // namespace HSServer
