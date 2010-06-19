#ifndef HIGHSCOREREQUESTHANDLER_H_INCLUDED
#define HIGHSCOREREQUESTHANDLER_H_INCLUDED


#include "Exceptions.h"
#include "RequestMethod.h"
#include "Utils.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Data/Session.h"
#include <map>
#include <string>
#include <boost/scoped_ptr.hpp>


namespace HSServer
{

    class RequestHandler : public Poco::Net::HTTPRequestHandler
    {
    public:
        RequestHandler(RequestMethod inRequestMethod,
                       const std::string & inLocation,
                       const std::string & inResponseContentType);

        /**
         * The content type of the response.
         * Can be text/plain, text/html, text/xml, ...
         */
        const std::string & getResponseContentType() const;

        /**
         * Returns the path part of the request uri.
         * For example "/hs".
         */
        const std::string & getPath() const;

        /**
         * Read the request and generate the response.
         * Must be implemented by subclass.
         */
        virtual void handleRequest(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse) = 0;

        std::string getSimpleHTML(const std::string & inTitle, const std::string & inBody);

        Poco::Data::Session mSession;

    private:
        RequestMethod mRequestMethod;
        std::string mLocation;
        std::string mResponseContentType;
    };


    class HTMLResponder : public RequestHandler
    {
    public:
        HTMLResponder(RequestMethod inRequestMethod,
                      const std::string & inLocation);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse) = 0;
    };


    class XMLResponder : public RequestHandler
    {
    public:
        XMLResponder(RequestMethod inRequestMethod, const std::string & inLocation);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse) = 0;
    };


    class PlainTextResponder : public RequestHandler
    {
    public:
        PlainTextResponder(RequestMethod inRequestMethod, const std::string & inLocation);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse) = 0;
    };

        
    class HTMLErrorResponse : public HTMLResponder
    {
    public:        
        HTMLErrorResponse(const std::string & inErrorMessage);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);
        std::string mErrorMessage;
    };

        
    class GetHighScoreHTML : public HTMLResponder
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hs"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        GetHighScoreHTML();

        void getRows(const Poco::Data::RecordSet & inRecordSet, std::string & outRows);
    };

        
    class GetHighScoreXML : public XMLResponder
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hs.xml"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        GetHighScoreXML();
    };

        
    class GetHallOfFameXML : public XMLResponder
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hof.xml"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        GetHallOfFameXML();
    };


    class GetAddHighScore : public HTMLResponder
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);        

        static const char * GetLocation() { return "/hs/add"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        GetAddHighScore();
    };


    class PostHighScore : public PlainTextResponder
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hs"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Post; }

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& inResponse);

    private:
        PostHighScore();
    };


    class CommitSucceeded : public HTMLResponder
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
