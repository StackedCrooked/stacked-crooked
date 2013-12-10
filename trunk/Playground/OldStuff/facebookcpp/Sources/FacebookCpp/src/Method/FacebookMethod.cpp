#include "FacebookCpp/Method/FacebookMethod.h"


using namespace FacebookCpp;
using namespace Poco;
using namespace std;



FacebookMethod::FacebookMethod(const string & inAPIMethod, const AbstractDelegate<ErrorResponse> & inErrorCallback):
	mAPIMethod(inAPIMethod),
	mErrorCallback(inErrorCallback)
{
	mParams["method"] = mAPIMethod;
	mParams["v"] = API_VERSION;
}


FacebookMethod::~FacebookMethod()
{
}


FacebookMethod::Params FacebookMethod::getParams() const
{
	return mParams;
}