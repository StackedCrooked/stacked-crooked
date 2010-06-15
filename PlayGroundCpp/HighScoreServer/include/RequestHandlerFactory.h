#ifndef HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED
#define HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED


#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"
#include <map>
#include <boost/bind.hpp>
#include <boost/function.hpp>


namespace HSServer
{
    class RequestHandler;

    class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
    {
    public:
        template<class T>
        void registerRequestHandler()
        {
            mFactoryFunctions.insert(std::make_pair(T::Location(), boost::bind(T::Create, _1)));
        }

	    Poco::Net::HTTPRequestHandler * createRequestHandler(const Poco::Net::HTTPServerRequest & request);

    private:
        typedef boost::function<RequestHandler*(const Poco::Net::HTTPServerRequest &)> FactoryFunction;
        typedef std::map<std::string, FactoryFunction> FactoryFunctions;
        FactoryFunctions mFactoryFunctions;
    };

} // namespace HSServer


#endif // HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED
