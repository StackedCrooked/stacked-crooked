#ifndef SQLREQUESTGENERICHANDLER_H_INLUDED
#define SQLREQUESTGENERICHANDLER_H_INLUDED


#include "RequestHandler.h"
#include "ContentType.h"
#include "Location.h"
#include "Renderer.h"
#include "RequestMethod.h"


namespace HSServer
{

    template<ContentType inContentType, Location inLocation>
    struct GetRenderer_t
    {
    };

    template <>
    struct GetRenderer_t<ContentType_TextHTML, Location_hs>
    {
        typedef HTMLRenderer Type;

        static const char * GetCollectionTitle() { return "High Scores"; }

        static const char * GetRecordTitle() { return ""; }
    };

    template <>
    struct GetRenderer_t<ContentType_TextHTML, Location_hof>
    {
        typedef HTMLRenderer Type;

        static const char * GetCollectionTitle() { return "Hall of Fame"; }

        static const char * GetRecordTitle() { return ""; }
    };

    template <>
    struct GetRenderer_t<ContentType_ApplicationXML, Location_hs>
    {
        typedef XMLRenderer Type;

        static const char * GetCollectionTitle() { return "highscores"; }

        static const char * GetRecordTitle() { return "hs"; }
    };

    template <>
    struct GetRenderer_t<ContentType_ApplicationXML, Location_hof>
    {
        typedef XMLRenderer Type;

        static const char * GetCollectionTitle() { return "hof"; }

        static const char * GetRecordTitle() { return "hs"; }
    };

    template <>
    struct GetRenderer_t<ContentType_TextPlain, Location_hs>
    {
        typedef PlainTextRenderer Type;

        static const char * GetCollectionTitle() { return "High Scores"; }

        static const char * GetRecordTitle() { return ""; }
    };

    template <>
    struct GetRenderer_t<ContentType_TextPlain, Location_hof>
    {
        typedef PlainTextRenderer Type;

        static const char * GetCollectionTitle() { return "Hall of Fame"; }

        static const char * GetRecordTitle() { return ""; }
    };

    template<RequestMethod inRequestMethod, Location inLocation>
    struct GetSQL_t
    {
    };

    template<>
    struct GetSQL_t<RequestMethod_Get, Location_hs>
    {
        static const char * GetValue() { return "SELECT * FROM HighScores"; }
    };

    template<>
    struct GetSQL_t<RequestMethod_Get, Location_hof>
    {
        static const char * GetValue() { return "SELECT Name, Score FROM HighScores ORDER BY Score DESC LIMIT 10"; }
    };


    template<RequestMethod inRequestMethod, ContentType inContentType, Location inLocation>
    class SQLRequestGenericHandler : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest)
        { return new SQLRequestGenericHandler<inRequestMethod, inContentType, inLocation>(); }

        SQLRequestGenericHandler() :
            RequestHandler(inRequestMethod,
                           GetLocation(),
                           inContentType)
        {
        }

        static RequestMethod GetRequestMethod() { return inRequestMethod; }
        
        static ContentType GetContentType() { return inContentType; }

        static const char * GetLocation() { return GetLocation_t<inLocation>::GetValue(); }

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
        {
            Poco::Data::Statement select(mSession);
            select << GetSQL_t<inRequestMethod, inLocation>::GetValue();
            select.execute();
            Poco::Data::RecordSet rs(select);

            typedef GetRenderer_t<inContentType, inLocation> RenderHelper;
            typedef typename RenderHelper::Type Renderer;
            Renderer renderer(RenderHelper::GetCollectionTitle(),
                              RenderHelper::GetRecordTitle(),
                              rs);

            std::stringstream ss;
            renderer.render(ss);

            std::string response = ss.str();
            outResponse.setContentLength(response.size());
            outResponse.send() << response;
        }
    };

    typedef SQLRequestGenericHandler<RequestMethod_Get, ContentType_TextHTML,       Location_hs > Get_TextHTML_hs;
    typedef SQLRequestGenericHandler<RequestMethod_Get, ContentType_ApplicationXML, Location_hs > Get_ApplicationXML_hs;
    typedef SQLRequestGenericHandler<RequestMethod_Get, ContentType_TextPlain,      Location_hs > Get_TextPlain_hs;

    typedef SQLRequestGenericHandler<RequestMethod_Get, ContentType_TextHTML,       Location_hof> Get_TextHTML_hof;
    typedef SQLRequestGenericHandler<RequestMethod_Get, ContentType_ApplicationXML, Location_hof> Get_ApplicationXML_hof;
    typedef SQLRequestGenericHandler<RequestMethod_Get, ContentType_TextPlain,      Location_hof> Get_TextPlain_hof;

} // namespace HSServer


#endif // SQLREQUESTGENERICHANDLER_H_INLUDED
