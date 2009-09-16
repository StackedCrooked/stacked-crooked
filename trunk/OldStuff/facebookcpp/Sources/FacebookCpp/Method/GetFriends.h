#ifndef __FacebookCpp_GetFriends_h__
#define __FacebookCpp_GetFriends_h__


#include "FacebookCpp/Method/FacebookSessionMethod.h"
#include <string>
#include <vector>


namespace FacebookCpp
{


class GetFriends : public FacebookSessionMethod
{
public:
	typedef std::vector<std::string> ResponseType;

	GetFriends(const Session & inSession, const Poco::AbstractDelegate<ResponseType> & inSuccessCallback, const Poco::AbstractDelegate<ErrorResponse> & inErrorCallback);

	virtual void handleResponse(std::istream & inputstream) const;

private:
	FacebookCpp::FbSharedCallback<ResponseType> mSuccessCallback;
};


}


#endif // __FacebookCpp_GetFriends_h__