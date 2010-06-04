#ifndef HIGHSCOREREQUESTHANDLER_H_INCLUDED
#define HIGHSCOREREQUESTHANDLER_H_INCLUDED


#include "Poco/Net/HTTPRequestHandler.h"
#include <map>


namespace HSServer
{

    typedef std::map<std::string, std::string> Args;

    void GetArgs(const std::string & inURI, Args & outArgs);
        
    class GetAllHighScores : public Poco::Net::HTTPRequestHandler
    {
    public:	
	    /**
         * Returns a HTML document with the HighScores
         */
        void handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                           Poco::Net::HTTPServerResponse& inResponse);
    };

    class AddHighScore : public Poco::Net::HTTPRequestHandler
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
