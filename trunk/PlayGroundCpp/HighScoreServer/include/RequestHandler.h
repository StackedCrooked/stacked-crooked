#ifndef HIGHSCOREREQUESTHANDLER_H_INCLUDED
#define HIGHSCOREREQUESTHANDLER_H_INCLUDED


#include "Exceptions.h"
#include "RequestMethod.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Data/Session.h"
#include <map>
#include <string>
#include <boost/scoped_ptr.hpp>


namespace HSServer
{

    typedef std::map<std::string, std::string> Args;

    class RequestHandler : public Poco::Net::HTTPRequestHandler
    {
    public:
        RequestHandler(RequestMethod inRequestMethod, const std::string & inLocation, const std::string & inResponseContentType);

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
        RequestMethod mRequestMethod;
        std::string mLocation;
        std::string mResponseContentType;
    };

        
    class ErrorRequestHandler : public RequestHandler
    {
    public:        
        ErrorRequestHandler(const std::string & inErrorMessage);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);
        std::string mErrorMessage;
    };

        
    class GetHighScore : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hs"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        GetHighScore();

        void getRows(const Poco::Data::RecordSet & inRecordSet, std::string & outRows);
    };


    class GetAddHighScore : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);        

        static const char * GetLocation() { return "/hs/add"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        GetAddHighScore();
    };


    class PostHighScore : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hs"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Post; }

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        PostHighScore();
    };


    class CommitSucceeded : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hs/commit-succeeded"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        CommitSucceeded(const std::string & inName, const std::string & inScore);
        std::string mName;
        std::string mScore;
    };

} // HighScoreServer


#endif // HIGHSCOREREQUESTHANDLER_H_INCLUDED
