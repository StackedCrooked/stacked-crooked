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


    class HighScoreRequestHandler : public Poco::Net::HTTPRequestHandler
    {
    public:
        HighScoreRequestHandler();

        void handleRequest(Poco::Net::HTTPServerRequest& inRequest,
                           Poco::Net::HTTPServerResponse& inResponse);

        
        static void GetArgs(const std::string & inURI, Args & outArgs);

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr) = 0;

        virtual std::string getContentType() const;

    private:
        Poco::Data::Session mSession;
    };


    class HighScore
    {
    public:
        HighScore(const std::string & inName, int inScore) : mName(inName), mScore(inScore) {}

        const std::string & name() const { return mName; }

        int score() const { return mScore; }

    private:
        std::string mName;
        int mScore;
    };

        
    class DefaultRequestHandler : public HighScoreRequestHandler
    {
    public:        
        static HighScoreRequestHandler * Create(const std::string & inURI);

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

    private:
        DefaultRequestHandler() {}
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

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

    private:
        GetAllHighScores() {}

        void getRows(const Poco::Data::RecordSet & inRecordSet, std::string & outRows);
    };


    class AddHighScore : public HighScoreRequestHandler
    {
    public:
        static HighScoreRequestHandler * Create(const std::string & inURI);

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

    private:
        AddHighScore();
    };


    class CommitHighScore : public HighScoreRequestHandler
    {
    public:
        static HighScoreRequestHandler * Create(const std::string & inURI);

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

        virtual std::string getContentType() const;

    private:
        CommitHighScore(const HighScore & inHighScore);
        HighScore mHighScore;
    };


    class CommitSucceeded : public HighScoreRequestHandler
    {
    public:
        static HighScoreRequestHandler * Create(const std::string & inURI);

    protected:
        virtual void generateResponse(Poco::Data::Session & inSession, std::ostream & ostr);

    private:
        CommitSucceeded(const HighScore & inHighScore);
        HighScore mHighScore;
    };

} // HighScoreServer


#endif // HIGHSCOREREQUESTHANDLER_H_INCLUDED
