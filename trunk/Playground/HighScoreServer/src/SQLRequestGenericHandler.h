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

    template<Method _Method, ResourceId _ResourceId>
    struct SelectQueryPolicy
    {
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


} // namespace HSServer


#endif // SQLREQUESTGENERICHANDLER_H_INLUDED
