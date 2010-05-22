#ifndef HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED
#define HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED


#include "HighScoreRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"


namespace HSServer
{

    class HighScoreRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
    {
    public:
	    Poco::Net::HTTPRequestHandler * createRequestHandler(const Poco::Net::HTTPServerRequest & request);
    };

} // namespace HSServer


#endif // HIGHSCOREREQUESTHANDLERFACTORY_H_INCLUDED
