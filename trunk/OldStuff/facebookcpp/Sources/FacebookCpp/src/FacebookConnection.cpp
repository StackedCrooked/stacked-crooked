#include "FacebookConnection.h"
#include "FacebookUtil.h"
#include <sstream>
#include <iostream>


using namespace FacebookCpp;
using namespace std;


FacebookConnection::FacebookConnection(const string& inApiKey, const string& inSecret, const string& inHost,	const string& inUrl):
	mApiKey(inApiKey),
	mSecret(inSecret),
	mHost(inHost),
	mUrl(inUrl)
{
}


FacebookConnection::~FacebookConnection()
{
}


const string & FacebookConnection::apiKey() const
{
	return mApiKey;
}


const string & FacebookConnection::host() const
{
	return mHost;
}


const string & FacebookConnection::secret() const
{
	return mSecret;
}


const string & FacebookConnection::url() const
{
	return mUrl;
}


void FacebookConnection::login(const string & inToken) const
{
	string theURL = "http://www.facebook.com/login.php?api_key=" + mApiKey + "&v=1.0&auth_token=" + inToken;
	launchBrowser(theURL);
}


string FacebookConnection::generateParams( const FacebookMethod& inMethod) const
{
	return FacebookCpp::generateParams(inMethod, mApiKey, mSecret);
}


string FacebookConnection::generateParams( const FacebookSessionMethod & inMethod) const
{
	return FacebookCpp::generateParams(inMethod, mApiKey);
}