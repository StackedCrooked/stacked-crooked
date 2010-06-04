#include "HighScoreRequestHandlerFactory.h"
#include "Poco/String.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Net/HTTPRequestHandler.h"


namespace HSServer
{

    Poco::Net::HTTPRequestHandler *
    HighScoreRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& inRequest)
    {
        if (inRequest.getURI().find("/method=GetAllHighScores") == 0)
        {
            return new GetAllHighScores;
        }
        else if (inRequest.getURI().find("/method=AddHighScore") == 0)
        {
            return new AddHighScore;
        }
        return 0;
    }

} // namespace HSServer
