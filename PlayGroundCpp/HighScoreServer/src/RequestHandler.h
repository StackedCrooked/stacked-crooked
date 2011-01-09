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

        Poco::Data::Session mSession;

    private:
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
        inline static ResourceId GetResourceId()
        { return _ResourceId; }
        
        inline static Method GetMethod()
        { return _Method; }

        inline static ContentType GetContentType()
        { return _ContentType; }

        static RequestHandler * Create()
        { return new SubClass; }

        GenericRequestHandler() :
            RequestHandler(_ResourceId, _Method, _ContentType)
        {
        }

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse) = 0;
    };


    class GetHighScorePostForm : public GenericRequestHandler<GetHighScorePostForm,
                                                              ResourceId_HighScorePostForm,
                                                              Method_Get,
                                                              ContentType_TextHTML>
    {
    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);
    };

    
    class GetHighScoreDeleteForm : public GenericRequestHandler<GetHighScoreDeleteForm,
                                                                ResourceId_HighScoreDeleteForm,
                                                                Method_Delete,
                                                                ContentType_TextHTML>
    {
    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);
    };

    
    class PostHightScore : public GenericRequestHandler<PostHightScore,
                                                        ResourceId_HighScore,
                                                        Method_Post,
                                                        ContentType_TextPlain>
    {
    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);
    };

    
    class DeleteHighScore : public GenericRequestHandler<DeleteHighScore,
                                                         ResourceId_HighScore,
                                                         Method_Delete,
                                                         ContentType_TextPlain>
    {
    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);
    };

} // HighScoreServer


#endif // HIGHSCOREREQUESTHANDLER_H_INCLUDED
