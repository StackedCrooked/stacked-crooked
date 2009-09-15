#ifndef __FacebookCpp_CreateAuthToken_h__
#define __FacebookCpp_CreateAuthToken_h__


#include "FacebookCpp/Method/FacebookMethod.h"
#include "FacebookCpp/Entity/AuthToken.h"


namespace FacebookCpp
{


class CreateAuthToken : public FacebookMethod
{
public:
	typedef AuthToken ResponseType;

	CreateAuthToken(const Poco::AbstractDelegate<ResponseType> & inSuccessCallback, const Poco::AbstractDelegate<ErrorResponse> & inErrorCallback);

	virtual void handleResponse(std::istream & inputstream) const;

private:
	FacebookCpp::FbSharedCallback<ResponseType> mSuccessCallback;
	
};


}


#endif // __FacebookCpp_CreateAuthToken_h__