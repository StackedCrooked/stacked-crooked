#include "FacebookCpp/Method/GetFriends.h"
#include "FacebookCpp/FacebookUtil.h"


using namespace FacebookCpp;
using namespace Poco;
using namespace std;


GetFriends::GetFriends(const Session & inSession, const AbstractDelegate<ResponseType> & inSuccessCallback, const AbstractDelegate<ErrorResponse> & inErrorCallback):
	FacebookSessionMethod(inSession, "Friends.get", inErrorCallback),
	mSuccessCallback(inSuccessCallback)
{
}


void GetFriends::handleResponse(istream & inputstream) const
{
	KeyValue values;
	vector<string> friends = parseSimpleXmlElements(inputstream, "Friends_get_response_elt", values);
	if(isErrorResponse(values))
	{
		mErrorCallback.call(ErrorResponse::makeErrorResponseFromKeyValuePairs(values));
	}
	else
	{
		mSuccessCallback.call(ResponseType(friends));
	}
}