#include "HighScoreRequestHandlerFactory.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/Application.h"


namespace HSServer
{

    Poco::Net::HTTPRequestHandler *
    HighScoreRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& inRequest)
    {
        // Log the request
        Poco::Util::Application::instance().logger().information("Request with uri: " + inRequest.getURI());

        if (inRequest.getURI().find("/method=GetAllHighScores") == 0)
        {
            return GetAllHighScores::Create(inRequest.getURI());
        }
        else if (inRequest.getURI().find("/method=AddHighScore") == 0)
        {
            return AddHighScore::Create(inRequest.getURI());
        }
        else
        {
            return DefaultRequestHandler::Create(inRequest.getURI());
        }
    }

} // namespace HSServer
