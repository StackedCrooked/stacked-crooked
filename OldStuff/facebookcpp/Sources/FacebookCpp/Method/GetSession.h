#ifndef __FacebookCpp_GetSession_h__
#define __FacebookCpp_GetSession_h__


#include "FacebookCpp/Method/FacebookMethod.h"
#include "FacebookCpp/Entity/Session.h"


namespace FacebookCpp
{


class GetSession : public FacebookMethod
{
public:
	typedef Session ResponseType;

	GetSession(const std::string & inToken, const Poco::AbstractDelegate<ResponseType> & inSuccessCallback, const Poco::AbstractDelegate<ErrorResponse> & inErrorCallback);

	virtual void handleResponse(std::istream & inputstream) const;

private:
	FacebookCpp::FbSharedCallback<ResponseType> mSuccessCallback;

};


}


#endif // __FacebookCpp_GetSession_h__