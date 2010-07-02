#ifndef GENERICREQUESTHANDLER_H_INLUDED
#define GENERICREQUESTHANDLER_H_INLUDED


#include "RequestHandler.h"
#include "ContentType.h"
#include "Location.h"
#include "Renderer.h"
#include "RequestMethod.h"


namespace HSServer
{

    template<ContentType inContentType, Location inLocation>
    struct GetRendererT
    {
    };

    template <>
    struct GetRendererT<ContentType_TextHTML, Location_hs>
    {
        typedef HTMLRenderer Type;

        static const char * GetCollectionTitle() { return "High Scores"; }

        static const char * GetRecordTitle() { return ""; }
    };

    template <>
    struct GetRendererT<ContentType_TextHTML, Location_hof>
    {
        typedef HTMLRenderer Type;

        static const char * GetCollectionTitle() { return "Hall of Fame"; }

        static const char * GetRecordTitle() { return ""; }
    };

    template <>
    struct GetRendererT<ContentType_ApplicationXML, Location_hs>
    {
        typedef XMLRenderer Type;

        static const char * GetCollectionTitle() { return "highscores"; }

        static const char * GetRecordTitle() { return "hs"; }
    };

    template <>
    struct GetRendererT<ContentType_ApplicationXML, Location_hof>
    {
        typedef XMLRenderer Type;

        static const char * GetCollectionTitle() { return "hof"; }

        static const char * GetRecordTitle() { return "hs"; }
    };

    template<RequestMethod inRequestMethod, Location inLocation>
    struct GetSQL
    {
    };

    template<>
    struct GetSQL<RequestMethod_Get, Location_hs>
    {
        static const char * GetValue() { return "SELECT * FROM HighScores"; }
    };

    template<>
    struct GetSQL<RequestMethod_Get, Location_hof>
    {
        static const char * GetValue() { return "SELECT Name, Score FROM HighScores ORDER BY Score DESC LIMIT 10"; }
    };


    template<RequestMethod inRequestMethod, ContentType inContentType, Location inLocation>
    class GenericRequestHandler : public RequestHandler
    {
    public:
        static RequestHandler * Create(const Poco::Net::HTTPServerRequest & inRequest)
        { return new GenericRequestHandler<inRequestMethod, inContentType, inLocation>(); }

        GenericRequestHandler() :
            RequestHandler(inRequestMethod,
                           GetLocation(),
                           inContentType)
        {
        }

        static RequestMethod GetRequestMethod() { return inRequestMethod; }
        
        static ContentType GetContentType() { return inContentType; }

        static const char * GetLocation() { return GetLocationT<inLocation>::GetValue(); }

        virtual void generateResponse(Poco::Net::HTTPServerRequest& inRequest, Poco::Net::HTTPServerResponse& outResponse)
        {
            Poco::Data::Statement select(mSession);
            select << GetSQL<inRequestMethod, inLocation>::GetValue();
            select.execute();
            Poco::Data::RecordSet rs(select);

            typedef GetRendererT<inContentType, inLocation> RenderHelper;
            typedef RenderHelper::Type Renderer;
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

    typedef GenericRequestHandler<RequestMethod_Get, ContentType_TextHTML,       Location_hs > Get_TextHTML_hs;
    typedef GenericRequestHandler<RequestMethod_Get, ContentType_ApplicationXML, Location_hs > Get_ApplicationXML_hs;
    typedef GenericRequestHandler<RequestMethod_Get, ContentType_TextHTML,       Location_hof> Get_TextHTML_hof;
    typedef GenericRequestHandler<RequestMethod_Get, ContentType_ApplicationXML, Location_hof> Get_ApplicationXML_hof;

} // namespace HSServer


#endif // GENERICREQUESTHANDLER_H_INLUDED
