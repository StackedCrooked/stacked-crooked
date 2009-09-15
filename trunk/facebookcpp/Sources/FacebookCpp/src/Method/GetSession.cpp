#include "FacebookCpp/Method/GetSession.h"


using namespace FacebookCpp;
using namespace Poco;
using namespace std;


GetSession::GetSession(const string & inToken, const AbstractDelegate<ResponseType> & inSuccessCallback, const AbstractDelegate<ErrorResponse> & inErrorCallback):
	FacebookMethod("facebook.auth.getSession", inErrorCallback),
	mSuccessCallback(inSuccessCallback)
{
	mParams["auth_token"] = inToken;
}


void GetSession::handleResponse(istream & inputstream) const
{
	KeyValue values = parseSimpleXml(inputstream);
	if(isErrorResponse(values))
	{
		mErrorCallback.call(ErrorResponse::makeErrorResponseFromKeyValuePairs(values));
	}
	else
	{
		mSuccessCallback.call(ResponseType(values["session_key"], values["uid"], values["expires"], values["secret"]));
	}
}