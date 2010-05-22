#ifndef HIGHSCOREREQUESTHANDLER_H_INCLUDED
#define HIGHSCOREREQUESTHANDLER_H_INCLUDED


#include "Poco/Net/HTTPRequestHandler.h"


namespace HSServer
{

    class HighScoreRequestHandler: public Poco::Net::HTTPRequestHandler
    {
    public:	
	    /**
         * Returns a HTML document with the HighScores
         */
        void handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                           Poco::Net::HTTPServerResponse& inResponse);
    };

} // HighScoreServer


#endif // HIGHSCOREREQUESTHANDLER_H_INCLUDED
