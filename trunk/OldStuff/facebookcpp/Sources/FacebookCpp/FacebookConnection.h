#ifndef __FacebookCpp_FacebookConnection_h__
#define __FacebookCpp_FacebookConnection_h__


#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "FacebookCpp/Method/FacebookMethod.h"
#include "Poco/ActiveMethod.h"
#include "Poco/Bugcheck.h"
#include "Poco/Net/HttpClientSession.h"
#include "Poco/Net/HttpRequest.h"
#include "Poco/Net/HttpResponse.h"
#include "Poco/Thread.h"
#include "Poco/Timer.h"
#include "Poco/Void.h"


namespace FacebookCpp
{


class FacebookConnection
{
public:

	FacebookConnection(
		const std::string& api_key,
		const std::string& secret,
		const std::string& host = "api.facebook.com",
		const std::string& url = "http://api.facebook.com/restserver.php");

	~FacebookConnection();

	const std::string & apiKey() const;

	const std::string & host() const;
	
	const std::string & secret() const;
	
	const std::string & url() const;
	
	void login(const std::string & inToken) const;

	template<class FacebookMethodT>
	void callMethod(FacebookMethodT inMethod, bool inExecuteInSeperateThread);

private:
	std::string generateParams( const FacebookMethod& inMethod) const;

	std::string generateParams( const FacebookSessionMethod & inMethod) const;

	template<class FacebookMethodT>
	Poco::Void callMethodBlocking(FacebookMethodT inMethod);

	std::string mApiKey;
	std::string mSecret;
	std::string mUrl;
	std::string mHost;

};




template<class FacebookMethodT>
void FacebookConnection::callMethod(FacebookMethodT inMethod, bool inExecuteInSeperateThread)
{
	if(inExecuteInSeperateThread)
	{
		Poco::ActiveMethod<Poco::Void, FacebookMethodT, FacebookConnection> activeMethod(this, &FacebookConnection::callMethodBlocking);
		Poco::ActiveResult<Poco::Void> result = activeMethod(inMethod);
	}
	else
	{
		callMethodBlocking(inMethod);
	}
};


template<class FacebookMethodT>
Poco::Void FacebookConnection::callMethodBlocking(FacebookMethodT inMethod)
{
	std::string params = generateParams(inMethod);
	std::string theUri = mUrl + "?" + params;
	Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, theUri);
	Poco::Net::HTTPClientSession theClientSession(mHost);
	theClientSession.sendRequest(req);
	Poco::Net::HTTPResponse response;
	inMethod.handleResponse(theClientSession.receiveResponse(response));
	return Poco::Void();
}


} // namespace FacebookCpp


#endif // __FacebookCpp_FacebookConnection_h__

