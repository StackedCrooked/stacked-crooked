#ifndef HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED
#define HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED


#include "RequestHandler.h"
#include "RequestHandlerId.h"
#include "RequestMethod.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"
#include <iomanip>
#include <iostream>
#include <map>
#include <boost/bind.hpp>
#include <boost/function.hpp>


namespace HSServer
{

    // Get the handler id from its type. Used for registration.
    template<class _GenericRequestHandler>
    RequestHandlerId GetRequestHandlerId()
    {
        return RequestHandlerId(_GenericRequestHandler::GetResourceId(),
                                _GenericRequestHandler::GetMethod(),
                                _GenericRequestHandler::GetContentType());
    }

    // Get the handler id of incoming HTTP request.
    RequestHandlerId GetRequestHandlerId(const Poco::Net::HTTPServerRequest & inRequest);


    static bool fHeadingIsPrinted = false;

    class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
    {
    public:
        template<class _GenericRequestHandler>
        void registerRequestHandler()
        {
            if (fHeadingIsPrinted == false)
            {
                std::cout << std::endl;
                std::cout << std::setw(17) << std::left << "Location";
                std::cout << std::setw(13) << std::left << "HTTP Method";
                std::cout << std::setw(17) << std::left << "Content Type";
                std::cout << std::endl;
                std::cout << std::setw(17) << std::left << std::string(std::string("Location").size(), '=');
                std::cout << std::setw(13) << std::left << std::string(std::string("HTTP Method").size(), '=');
                std::cout << std::setw(17) << std::left << std::string(std::string("Content Type").size(), '=');
                std::cout << std::endl;
                fHeadingIsPrinted = true;
            }
            std::cout << std::setw(17) << std::left << std::string("/" + ResourceManager::Instance().getResourceLocation(_GenericRequestHandler::GetResourceId()));
            std::cout << std::setw(13) << std::left << ToString(_GenericRequestHandler::GetMethod());
            std::cout << std::setw(17) << std::left << ToString(_GenericRequestHandler::GetContentType());
            std::cout << std::endl;
            mFactoryFunctions.insert(
                std::make_pair(GetRequestHandlerId<_GenericRequestHandler>(),
                boost::bind(_GenericRequestHandler::Create)));
        }

        Poco::Net::HTTPRequestHandler * createRequestHandler(const Poco::Net::HTTPServerRequest & request);

    private:
        typedef boost::function<RequestHandler*()> FactoryFunction;
        typedef std::map<RequestHandlerId, FactoryFunction> FactoryFunctions;
        FactoryFunctions mFactoryFunctions;
    };

} // namespace HSServer


#endif // HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED
