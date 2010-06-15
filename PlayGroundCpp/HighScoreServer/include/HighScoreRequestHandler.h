#ifndef HIGHSCOREREQUESTHANDLER_H_INCLUDED
#define HIGHSCOREREQUESTHANDLER_H_INCLUDED


#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Data/Session.h"
#include <map>
#include <string>


namespace HSServer
{

    typedef std::map<std::string, std::string> Args;


    class MissingArgumentException : public std::runtime_error
    {
    public:
        MissingArgumentException(const std::string & inMessage);
    };


    class HighScoreRequestHandler : public Poco::Net::HTTPRequestHandler
    {
    public:
        HighScoreRequestHandler(const std::string & inLocation, const std::string & inResponseContentType);

        const std::string & getResponseContentType() const;

        const std::string & getLocation() const;

        void handleRequest(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);
        
        static void GetArgs(const std::string & inURI, Args & outArgs);

        // Throws MissingArgumentException if not found.
        static const std::string & GetArg(const Args & inArgs, const std::string & inArg);

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr) = 0;

    private:
        Poco::Data::Session mSession;
        std::string mLocation;
        std::string mResponseContentType;
    };

        
    class DefaultRequestHandler : public HighScoreRequestHandler
    {
    public:        
        static HighScoreRequestHandler * Create(const std::string & inURI);

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

    private:
        DefaultRequestHandler();
    };

        
    class ErrorRequestHandler : public HighScoreRequestHandler
    {
    public:        
        ErrorRequestHandler(const std::string & inErrorMessage);

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);
        std::string mErrorMessage;
    };

        
    class GetAllHighScores : public HighScoreRequestHandler
    {
    public:
        static HighScoreRequestHandler * Create(const std::string & inURI);

        static const char * Location() { return "/hs/getall"; }

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

    private:
        GetAllHighScores();

        void getRows(const Poco::Data::RecordSet & inRecordSet, std::string & outRows);
    };


    class AddHighScore : public HighScoreRequestHandler
    {
    public:
        static HighScoreRequestHandler * Create(const std::string & inURI);        
        
        static const char * Location() { return "/hs/add"; }

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

    private:
        AddHighScore();
    };


    class CommitHighScore : public HighScoreRequestHandler
    {
    public:
        static HighScoreRequestHandler * Create(const std::string & inURI);

        static const char * Location() { return "/hs/commit"; }

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

    private:
        CommitHighScore(const std::string & inName, const std::string & inScore);
        std::string mName;
        std::string mScore;
    };


    class CommitSucceeded : public HighScoreRequestHandler
    {
    public:
        static HighScoreRequestHandler * Create(const std::string & inURI);

        static const char * Location() { return "/hs/commit-succeeded"; }

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

    private:
        CommitSucceeded(const std::string & inName, const std::string & inScore);
        std::string mName;
        std::string mScore;
    };

} // HighScoreServer


#endif // HIGHSCOREREQUESTHANDLER_H_INCLUDED
