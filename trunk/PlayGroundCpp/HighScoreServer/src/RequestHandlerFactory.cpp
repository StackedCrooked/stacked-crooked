#include "RequestHandlerFactory.h"
#include "ContentType.h"
#include "RequestHandler.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/Application.h"


namespace HSServer
{

    std::string GetLocation(const Poco::Net::HTTPServerRequest & inRequest)
    {
        return inRequest.getURI().substr(0, inRequest.getURI().find("?"));
    }


    /**
     * The request usually contains multiple content types. We select one
     * according the the following priority list:
     *   - text/html
     *   - text/xml
     *   - text/plain
     */
    ContentType SelectContentType(const Poco::Net::HTTPServerRequest & inRequest)
    {
        const std::string & acceptHeader = inRequest.get("Accept");

        if (acceptHeader.find("text/html") != std::string::npos)
        {
            return ContentType_TextHTML;
        }
        else if (acceptHeader.find("text/xml") != std::string::npos)
        {
            return ContentType_TextXML;
        }
        else if (acceptHeader.find("text/plain") != std::string::npos)
        {
            return ContentType_TextPlain;
        }
        throw std::runtime_error("Unsupported content type: " + acceptHeader);
    }


    RequestMethod GetRequestMethod(const Poco::Net::HTTPServerRequest & inRequest)
    {
        if (inRequest.getMethod() == "GET")
        {
            return RequestMethod_Get;
        }
        else if (inRequest.getMethod() == "POST")
        {
            return RequestMethod_Post;
        }
        throw std::runtime_error("Unsupported request method: " + inRequest.getMethod());
    }


    RequestHandlerId GetRequestHandlerId(const Poco::Net::HTTPServerRequest & inRequest)
    {        
        return RequestHandlerId(GetLocation(inRequest),
                                GetRequestMethod(inRequest),
                                SelectContentType(inRequest));
    }


    Poco::Net::HTTPRequestHandler *
    RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& inRequest)
    {
        // Log the request
        Poco::Logger & fLogger = Poco::Util::Application::instance().logger();

        try
        {
            fLogger.information("Request with uri: " + inRequest.getURI());        
            fLogger.information("Request id: " + ToString(GetRequestHandlerId(inRequest)));

            FactoryFunctions::iterator it = mFactoryFunctions.find(GetRequestHandlerId(inRequest));            
            if (it != mFactoryFunctions.end())
            {
                const FactoryFunction & ff(it->second);
                return ff(inRequest);
            }
        }
        catch (const std::exception & inException)
        {
            fLogger.error(inException.what());
        }
        return new HTMLErrorResponse("No handler for " + inRequest.getURI());
    }

} // namespace HSServer
