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

        
        if (inRequest.getURI() == "/hs/getall")
        {
            return GetAllHighScores::Create(inRequest.getURI());
        }
        else if (inRequest.getURI().find("/hs/add") == 0)
        {
            return AddHighScore::Create(inRequest.getURI());
        }
        else if (inRequest.getURI().find("/hs/commit-succeeded") == 0)
        {
            return CommitSucceeded::Create(inRequest.getURI());
        }
        else if (inRequest.getURI().find("/hs/commit") == 0)
        {
            return CommitHighScore::Create(inRequest.getURI());
        }
        else if (inRequest.getURI() == "/")
        {
            return DefaultRequestHandler::Create(inRequest.getURI());
        }
        else
        {
            return new ErrorRequestHandler("No handler for " + inRequest.getURI());
        }
    }

} // namespace HSServer
