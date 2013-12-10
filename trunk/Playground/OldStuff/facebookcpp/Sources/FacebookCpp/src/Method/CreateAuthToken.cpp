#include "FacebookCpp/Method/CreateAuthToken.h"


using namespace FacebookCpp;
using namespace Poco;
using namespace std;



CreateAuthToken::CreateAuthToken(const AbstractDelegate<ResponseType> & inSuccessCallback, const AbstractDelegate<ErrorResponse> & inErrorCallback):
	FacebookMethod("auth.createToken", inErrorCallback),
	mSuccessCallback(inSuccessCallback)
{
}


void CreateAuthToken::handleResponse(istream & inputstream) const
{
	KeyValue values = parseSimpleXml(inputstream);
	if(isErrorResponse(values))
	{
		mErrorCallback.call(ErrorResponse::makeErrorResponseFromKeyValuePairs(values));
	}
	else
	{
		mSuccessCallback.call(ResponseType(values["auth_createToken_response"]));
	}
}