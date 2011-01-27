#ifndef SQLREQUESTGENERICHANDLER_H_INLUDED
#define SQLREQUESTGENERICHANDLER_H_INLUDED


#include "RequestHandler.h"
#include "ContentType.h"
#include "ResourceId.h"
#include "Renderer.h"
#include "RequestMethod.h"


namespace HSServer
{

    template<ContentType _ContentType, ResourceId _ResourceId>
    struct TagNamingPolicy
    {
    };

    template <>
    struct TagNamingPolicy<ContentType_TextHTML, Resource_HighScore::Id>
    {
        typedef HTMLRenderer RendererType;

        static const char * GetCollectionTagName() { return "High Scores"; }

        static const char * GetItemTagName() { return ""; }
    };

    template <>
    struct TagNamingPolicy<ContentType_TextHTML, Resource_HallOfFame::Id>
    {
        typedef HTMLRenderer RendererType;

        static const char * GetCollectionTagName() { return "Hall of Fame"; }

        static const char * GetItemTagName() { return ""; }
    };

    template <>
    struct TagNamingPolicy<ContentType_ApplicationXML, Resource_HighScore::Id>
    {
        typedef XMLRenderer RendererType;

        static const char * GetCollectionTagName() { return "highscores"; }

        static const char * GetItemTagName() { return "hs"; }
    };

    template <>
    struct TagNamingPolicy<ContentType_ApplicationXML, Resource_HallOfFame::Id>
    {
        typedef XMLRenderer RendererType;

        static const char * GetCollectionTagName() { return "hof"; }

        static const char * GetItemTagName() { return "hs"; }
    };

    template <>
    struct TagNamingPolicy<ContentType_TextPlain, Resource_HighScore::Id>
    {
        typedef PlainTextRenderer RendererType;

        static const char * GetCollectionTagName() { return "High Scores"; }

        static const char * GetItemTagName() { return ""; }
    };

    template <>
    struct TagNamingPolicy<ContentType_TextPlain, Resource_HallOfFame::Id>
    {
        typedef PlainTextRenderer RendererType;

        static const char * GetCollectionTagName() { return "Hall of Fame"; }

        static const char * GetItemTagName() { return ""; }
    };

    template<Method _Method, ResourceId _ResourceId>
    struct SelectQueryPolicy
    {
    };

    template<>
    struct SelectQueryPolicy<Method_Get, Resource_HighScore::Id>
    {
        static const char * GetSelectQuery() { return "SELECT * FROM HighScores"; }
    };

    template<>
    struct SelectQueryPolicy<Method_Get, Resource_HallOfFame::Id>
    {
        static const char * GetSelectQuery() { return "SELECT Name, Score FROM HighScores ORDER BY Score DESC LIMIT 10"; }
    };


    template<class T, ResourceId _ResourceId, Method _Method, ContentType _ContentType>
    class SQLRequestGenericHandler :
        public GenericRequestHandler<T, _ResourceId, _Method, _ContentType>,
        public TagNamingPolicy<_ContentType, _ResourceId>,
        public SelectQueryPolicy<_Method, _ResourceId>
    {
    public:
        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest,
                                      Poco::Net::HTTPServerResponse& outResponse)
        {
            // Peform the SELECT query
            Poco::Data::Statement select(GenericRequestHandler<T, _ResourceId, _Method, _ContentType>::getSession());
            select << this->GetSelectQuery();
            select.execute();


            // Get the result.
            Poco::Data::RecordSet rs(select);


            // Create a textual representation for the result.
            typename TagNamingPolicy<_ContentType, _ResourceId>::RendererType renderer(
                this->GetCollectionTagName(),
                this->GetItemTagName(), rs);
            std::stringstream ss;
            renderer.render(ss);


            // Send the response.
            std::string response = ss.str();
            outResponse.setContentLength(response.size());
            outResponse.send() << response;
        }
    };


    class GetHighScore_HTML : public SQLRequestGenericHandler<
        GetHighScore_HTML,
        Resource_HighScore::Id,
        Method_Get,
        ContentType_TextHTML>
    {
    };


    class GetHighScore_XML : public SQLRequestGenericHandler<
        GetHighScore_XML,
        Resource_HighScore::Id,
        Method_Get,
        ContentType_ApplicationXML>
    {
    };


    class GetHighScore_Text : public SQLRequestGenericHandler<
        GetHighScore_Text,
        Resource_HighScore::Id,
        Method_Get,
        ContentType_TextPlain>
    {
    };


    class GetHallOfFame_HTML : public SQLRequestGenericHandler<
        GetHallOfFame_HTML,
        Resource_HallOfFame::Id,
        Method_Get,
        ContentType_TextHTML>
    {
    };


    class GetHallOfFame_XML : public SQLRequestGenericHandler<
        GetHallOfFame_XML,
        Resource_HallOfFame::Id,
        Method_Get,
        ContentType_ApplicationXML>
    {
    };


    class GetHallOfFame_Text : public SQLRequestGenericHandler<
        GetHallOfFame_Text,
        Resource_HallOfFame::Id,
        Method_Get,
        ContentType_TextPlain>
    {
    };


} // namespace HSServer


#endif // SQLREQUESTGENERICHANDLER_H_INLUDED
