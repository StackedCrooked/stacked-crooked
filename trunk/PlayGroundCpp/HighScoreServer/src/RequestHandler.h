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
        RequestHandler(RequestMethod inRequestMethod,
                       const std::string & inLocation,
                       ContentType inContentType);

        /**
         * The content type of the response.
         * Can be text/plain, text/html, application/xml, ...
         */
        ContentType contentType() const;

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


    template<class T>
    class CreationPolicy
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest)
        {
            return new T;
        }
    };


    template<ResourceId resourceId>
    class LocationPolicy
    {
    public:
        static const char * GetLocation() { return ToString(resourceId); }
    };


    template<RequestMethod _RequestMethod>
    class RequestMethodPolicy
    {
    public:
        static RequestMethod GetRequestMethod() { return _RequestMethod; }
    };


    class GetHighScorePostForm : public RequestHandler,
                                 public CreationPolicy<GetHighScorePostForm>,
                                 public LocationPolicy<ResourceId_HighScorePostForm>,
                                 public RequestMethodPolicy<RequestMethod_Get>
    {
    public:
        typedef GetHighScorePostForm ThisType;

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    private:
        friend class CreationPolicy<ThisType>;
        GetHighScorePostForm();
    };


    class GetHighScoreDeleteForm : public RequestHandler,
                                   public CreationPolicy<GetHighScoreDeleteForm>,
                                   public LocationPolicy<ResourceId_HighScoreDeleteForm>,
                                   public RequestMethodPolicy<RequestMethod_Get>
    {
    public:
        typedef GetHighScoreDeleteForm ThisType;

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    private:
        friend class CreationPolicy<ThisType>;
        GetHighScoreDeleteForm();
    };


    class PostHightScore : public RequestHandler,
                           public CreationPolicy<PostHightScore>,
                           public LocationPolicy<ResourceId_HighScore>,
                           public RequestMethodPolicy<RequestMethod_Post>
    {
    public:
        typedef PostHightScore ThisType;

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    private:
        friend class CreationPolicy<ThisType>;
        PostHightScore();
    };


    class DeleteHighScore : public RequestHandler,
                            public CreationPolicy<DeleteHighScore>,
                            public LocationPolicy<ResourceId_HighScore>,
                            public RequestMethodPolicy<RequestMethod_Delete>
    {
    public:
        typedef DeleteHighScore ThisType;

    protected:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);

    private:
        friend class CreationPolicy<ThisType>;
        DeleteHighScore();
    };


} // HighScoreServer


#endif // HIGHSCOREREQUESTHANDLER_H_INCLUDED
