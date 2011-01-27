#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED


#include "ContentType.h"
#include "RequestHandler.h"
#include "RequestHandlerFactory.h"
#include "RequestMethod.h"
#include "ResourceId.h"
#include "SQLRequestGenericHandler.h"


namespace HSServer {


//
// Define the resources
//
DEFINE_HTTP_RESOURCE(HighScore, "hs")
DEFINE_HTTP_RESOURCE(HighScorePostForm, "hs/post-form")
DEFINE_HTTP_RESOURCE(HighScorePostConfirmation, "hs/post-confirmation")
DEFINE_HTTP_RESOURCE(HighScoreDeleteForm, "hs/delete-form")
DEFINE_HTTP_RESOURCE(HighScoreDeleteConfirmation, "hs/delete-confirmation")
DEFINE_HTTP_RESOURCE(HallOfFame, "hof")
DEFINE_HTTP_RESOURCE(ErrorPage, "error-page")


class GetHighScorePostForm : public GenericRequestHandler<GetHighScorePostForm,
                                                          HighScorePostForm::Id,
                                                          Method_Get,
                                                          ContentType_TextHTML>
{
protected:
    virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);
};


class GetHighScoreDeleteForm : public GenericRequestHandler<GetHighScoreDeleteForm,
                                                            HighScoreDeleteForm::Id,
                                                            Method_Get,
                                                            ContentType_TextHTML>
{
protected:
    virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);
};


class PostHightScore : public GenericRequestHandler<PostHightScore,
                                                    HighScore::Id,
                                                    Method_Post,
                                                    ContentType_TextPlain>
{
protected:
    virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);
};


class DeleteHighScore : public GenericRequestHandler<DeleteHighScore,
                                                     HighScore::Id,
                                                     Method_Delete,
                                                     ContentType_TextPlain>
{
protected:
    virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse);
};

template <>
struct TagNamingPolicy<ContentType_TextHTML, HighScore::Id>
{
    typedef HTMLRenderer RendererType;

    static const char * GetCollectionTagName() { return "High Scores"; }

    static const char * GetItemTagName() { return ""; }
};

template <>
struct TagNamingPolicy<ContentType_TextHTML, HallOfFame::Id>
{
    typedef HTMLRenderer RendererType;

    static const char * GetCollectionTagName() { return "Hall of Fame"; }

    static const char * GetItemTagName() { return ""; }
};

template <>
struct TagNamingPolicy<ContentType_ApplicationXML, HighScore::Id>
{
    typedef XMLRenderer RendererType;

    static const char * GetCollectionTagName() { return "highscores"; }

    static const char * GetItemTagName() { return "hs"; }
};

template <>
struct TagNamingPolicy<ContentType_ApplicationXML, HallOfFame::Id>
{
    typedef XMLRenderer RendererType;

    static const char * GetCollectionTagName() { return "hof"; }

    static const char * GetItemTagName() { return "hs"; }
};

template <>
struct TagNamingPolicy<ContentType_TextPlain, HighScore::Id>
{
    typedef PlainTextRenderer RendererType;

    static const char * GetCollectionTagName() { return "High Scores"; }

    static const char * GetItemTagName() { return ""; }
};

template <>
struct TagNamingPolicy<ContentType_TextPlain, HallOfFame::Id>
{
    typedef PlainTextRenderer RendererType;

    static const char * GetCollectionTagName() { return "Hall of Fame"; }

    static const char * GetItemTagName() { return ""; }
};

template<>
struct SelectQueryPolicy<Method_Get, HighScore::Id>
{
    static const char * GetSelectQuery() { return "SELECT * FROM HighScores"; }
};

template<>
struct SelectQueryPolicy<Method_Get, HallOfFame::Id>
{
    static const char * GetSelectQuery() { return "SELECT Name, Score FROM HighScores ORDER BY Score DESC LIMIT 10"; }
};


class GetHighScore_HTML : public SQLRequestGenericHandler<
    GetHighScore_HTML,
    HighScore::Id,
    Method_Get,
    ContentType_TextHTML>
{
};


class GetHighScore_XML : public SQLRequestGenericHandler<
    GetHighScore_XML,
    HighScore::Id,
    Method_Get,
    ContentType_ApplicationXML>
{
};


class GetHighScore_Text : public SQLRequestGenericHandler<
    GetHighScore_Text,
    HighScore::Id,
    Method_Get,
    ContentType_TextPlain>
{
};


class GetHallOfFame_HTML : public SQLRequestGenericHandler<
    GetHallOfFame_HTML,
    HallOfFame::Id,
    Method_Get,
    ContentType_TextHTML>
{
};


class GetHallOfFame_XML : public SQLRequestGenericHandler<
    GetHallOfFame_XML,
    HallOfFame::Id,
    Method_Get,
    ContentType_ApplicationXML>
{
};


class GetHallOfFame_Text : public SQLRequestGenericHandler<
    GetHallOfFame_Text,
    HallOfFame::Id,
    Method_Get,
    ContentType_TextPlain>
{
};


void RegisterRequestHandlers(RequestHandlerFactory & ioRequestHandlerFactory);


} // namespace HSServer


#endif // CONFIG_H_INCLUDED
