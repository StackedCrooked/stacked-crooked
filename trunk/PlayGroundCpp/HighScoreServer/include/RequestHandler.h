#ifndef HIGHSCOREREQUESTHANDLER_H_INCLUDED
#define HIGHSCOREREQUESTHANDLER_H_INCLUDED


#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Data/Session.h"
#include <map>
#include <string>
#include <boost/scoped_ptr.hpp>


namespace HSServer
{

    typedef std::map<std::string, std::string> Args;


    class MissingArgumentException : public std::runtime_error
    {
    public:
        MissingArgumentException(const std::string & inMessage);
    };


    class RequestHandler : public Poco::Net::HTTPRequestHandler
    {
    public:
        RequestHandler(const std::string & inLocation, const std::string & inResponseContentType);

        const std::string & getResponseContentType() const;

        const std::string & getLocation() const;

        virtual void handleRequest(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);
        
        static void GetArgs(const std::string & inURI, Args & outArgs);

        // Throws MissingArgumentException if not found.
        static const std::string & GetArg(const Args & inArgs, const std::string & inArg);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse) = 0;

        Poco::Data::Session mSession;

    private:
        std::string mLocation;
        std::string mResponseContentType;
    };


    class Responder
    {
    public:

    };

        
    class DefaultRequestHandler : public RequestHandler
    {
    public:        
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        DefaultRequestHandler();
    };

        
    class ErrorRequestHandler : public RequestHandler
    {
    public:        
        ErrorRequestHandler(const std::string & inErrorMessage);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);
        std::string mErrorMessage;
    };

        
    class GetAllHighScores : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * Location() { return "/hs/getall"; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        GetAllHighScores();

        void getRows(const Poco::Data::RecordSet & inRecordSet, std::string & outRows);
    };


    class AddHighScoreImpl;


    class AddHighScore : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);        
        
        static const char * Location() { return "/hs/add"; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        // In case of a GET request
        AddHighScore(AddHighScoreImpl * inImpl);

        boost::scoped_ptr<AddHighScoreImpl> mImpl;
    };


    class AddHighScoreImpl
    {
    public:
        AddHighScoreImpl(const std::string & inContentType);

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest,
                                      Poco::Net::HTTPServerResponse& inResponse) = 0;

        const std::string & getContentType() const;

        void setSession(Poco::Data::Session * inSession);

    protected:
        std::string mContentType;
        Poco::Data::Session * mSession;
    };


    class AddHighScore_POST : public AddHighScoreImpl
    {
    public:
        AddHighScore_POST();

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);
    };


    class AddHighScore_GET : public AddHighScoreImpl
    {
    public:
        AddHighScore_GET();

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);
    };


    class CommitHighScore : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * Location() { return "/hs/commit"; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        CommitHighScore(const std::string & inName, const std::string & inScore);
        std::string mName;
        std::string mScore;
    };


    class CommitSucceeded : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * Location() { return "/hs/commit-succeeded"; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        CommitSucceeded(const std::string & inName, const std::string & inScore);
        std::string mName;
        std::string mScore;
    };

} // HighScoreServer


#endif // HIGHSCOREREQUESTHANDLER_H_INCLUDED
