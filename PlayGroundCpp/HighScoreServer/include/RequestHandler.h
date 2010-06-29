#ifndef HIGHSCOREREQUESTHANDLER_H_INCLUDED
#define HIGHSCOREREQUESTHANDLER_H_INCLUDED


#include "Exceptions.h"
#include "ContentType.h"
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
                       ContentType inContentType);

        /**
         * The content type of the response.
         * Can be text/plain, text/html, text/xml, ...
         */
        ContentType getContentType() const;

        /**
         * Returns the path part of the request uri.
         * For example "/hs".
         */
        const std::string & getPath() const;

        /**
         * Read the request and generate the response.
         * Must be implemented by subclass.
         */
        virtual void handleRequest(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse) = 0;

        std::string getSimpleHTML(const std::string & inTitle, const std::string & inBody);

        Poco::Data::Session mSession;

    private:
        RequestMethod mRequestMethod;
        std::string mLocation;
        ContentType mContentType;
    };

        
    class HTMLErrorResponse : public RequestHandler
    {
    public:        
        HTMLErrorResponse(const std::string & inErrorMessage);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);
        std::string mErrorMessage;
    };
        
    class GetHighScoreHTML : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hs"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }
        
        static ContentType GetContentType() { return ContentType_TextHTML; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    private:
        GetHighScoreHTML();
    };

        
    class GetHighScoreXML : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hs"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }
        
        static ContentType GetContentType() { return ContentType_TextXML; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    private:
        GetHighScoreXML();
    };

        
    class GetHallOfFameXML : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hof"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }
        
        static ContentType GetContentType() { return ContentType_TextXML; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    private:
        GetHallOfFameXML();
    };

        
    class GetHallOfFameHTML : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hof"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }
        
        static ContentType GetContentType() { return ContentType_TextHTML; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    private:
        GetHallOfFameHTML();
    };


    class GetAddHighScore : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);        

        static const char * GetLocation() { return "/hs/add"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }
        
        static ContentType GetContentType() { return ContentType_TextHTML; }

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    private:
        GetAddHighScore();
    };


    class PostHighScore : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hs"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Post; }
        
        static ContentType GetContentType() { return ContentType_TextPlain; }

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    private:
        PostHighScore();
    };


    class CommitSucceeded : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest);

        static const char * GetLocation() { return "/hs/commit-succeeded"; }

        static RequestMethod GetRequestMethod() { return RequestMethod_Get; }
        
        static ContentType GetContentType() { return ContentType_TextPlain; }

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    private:
        CommitSucceeded(const std::string & inName, const std::string & inScore);
        std::string mName;
        std::string mScore;
    };

} // HighScoreServer


#endif // HIGHSCOREREQUESTHANDLER_H_INCLUDED
