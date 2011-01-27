#ifndef HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED
#define HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED


#include "RequestHandler.h"
#include "RequestHandlerId.h"
#include "RequestMethod.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"
#include <map>
#include <boost/bind.hpp>
#include <boost/function.hpp>


namespace HSServer
{

    // Get the handler id from its type. Used for registration.
    template<class T>
    RequestHandlerId GetRequestHandlerId()
    {
        return RequestHandlerId(T::GetResourceId(), T::GetMethod());
    }

    // Get the handler id of incoming HTTP request.
    RequestHandlerId GetRequestHandlerId(const Poco::Net::HTTPServerRequest & inRequest);


    class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
    {
    public:
        template<class T>
        void registerRequestHandler()
        {
            mFactoryFunctions.insert(std::make_pair(GetRequestHandlerId<T>(), boost::bind(T::Create)));
        }

        Poco::Net::HTTPRequestHandler * createRequestHandler(const Poco::Net::HTTPServerRequest & request);

    private:
        typedef boost::function<RequestHandler*()> FactoryFunction;
        typedef std::map<RequestHandlerId, FactoryFunction> FactoryFunctions;
        FactoryFunctions mFactoryFunctions;
    };

} // namespace HSServer


#endif // HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED
