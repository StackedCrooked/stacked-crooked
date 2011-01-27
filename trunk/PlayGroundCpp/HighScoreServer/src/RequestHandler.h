#ifndef HIGHSCOREREQUESTHANDLER_H_INCLUDED
#define HIGHSCOREREQUESTHANDLER_H_INCLUDED


#include "Exceptions.h"
#include "ContentType.h"
#include "ResourceId.h"
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
        RequestHandler(ResourceId inResourceId,
                       Method inMethod,
                       ContentType inContentType);

        virtual ~RequestHandler() {}

        inline ResourceId resourceId() const { return mResourceId; }

        inline Method requestMethod() const { return mMethod; }

        inline ContentType contentType() const { return mContentType; }

        /**
         * Read the request and generate the response.
         * Must be implemented by subclass.
         */
        virtual void handleRequest(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse) = 0;

        Poco::Data::Session & getSession() { return mSession; }
        const Poco::Data::Session & getSession() const { return mSession; }

    private:
        Poco::Data::Session mSession;
        ResourceId mResourceId;
        Method mMethod;
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


    template<class SubClass, ResourceId _ResourceId, Method _Method, ContentType _ContentType>
    class GenericRequestHandler : public RequestHandler
    {
    public:
        static ResourceId GetResourceId()
        { return _ResourceId; }

        static Method GetMethod()
        { return _Method; }

        static ContentType GetContentType()
        { return _ContentType; }

        static RequestHandler * Create()
        { return new SubClass; }

        GenericRequestHandler() :
            RequestHandler(_ResourceId, _Method, _ContentType)
        {
        }

        virtual ~GenericRequestHandler() {}

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse) = 0;
    };

} // HighScoreServer


#endif // HIGHSCOREREQUESTHANDLER_H_INCLUDED
