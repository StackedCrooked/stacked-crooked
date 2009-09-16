#include "FacebookCpp/Method/FacebookSessionMethod.h"


using namespace FacebookCpp;
using namespace Poco;
using namespace std;


FacebookSessionMethod::FacebookSessionMethod(const Session & inSession, const string & inAPIMethod, const AbstractDelegate<ErrorResponse> & inErrorCallback):
	FacebookMethod(inAPIMethod, inErrorCallback),
	mSession(inSession)
{
		mParams["session_key"] = mSession.sessionKey();
		mParams["uid"] = mSession.uid();
		mParams["expires"] = mSession.expires();
		mParams["secret"] = mSession.secret();
		stringstream buf;
		buf << Timestamp().epochMicroseconds();
		mParams["call_id"] = buf.str();
}