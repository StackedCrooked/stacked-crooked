#ifndef HIGHSCOREREQUESTHANDLER_H_INCLUDED
#define HIGHSCOREREQUESTHANDLER_H_INCLUDED


#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Data/Session.h"
#include <map>
#include <string>


namespace HSServer
{

    class HighScoreRequestHandler : public Poco::Net::HTTPRequestHandler
    {
    public:
        HighScoreRequestHandler();

        void handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                           Poco::Net::HTTPServerResponse& inResponse);

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr) = 0;

    private:
        Poco::Data::Session mSession;
    };

        
    class GetAllHighScores : public HighScoreRequestHandler
    {
    public:        
        static GetAllHighScores * Create(const std::string & inURI);

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

    private:
        GetAllHighScores() {}
    };


    class AddHighScore : public HighScoreRequestHandler
    {
    public:
        static AddHighScore * Create(const std::string & inURI);

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

    private:
        AddHighScore(const std::string & inName, int inScore);
        std::string mName;
        int mScore;
    };

} // HighScoreServer


#endif // HIGHSCOREREQUESTHANDLER_H_INCLUDED
