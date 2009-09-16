#ifndef __FacebookCpp_users_getInfo__
#define __FacebookCpp_users_getInfo__


#include "FacebookCpp/Method/FacebookSessionMethod.h"
#include "XSD/facebook.hxx"


namespace FacebookCpp
{


class users_getInfo :	public FacebookSessionMethod
{
public:
	typedef std::auto_ptr< XSD::users_getInfo_response > ResponseType;

	users_getInfo
	(
		const Session & inSession,
		const std::vector<std::string> & inUids,
		const Poco::AbstractDelegate<ResponseType> & inSuccessCallback,
		const Poco::AbstractDelegate<ErrorResponse> & inErrorCallback
	);

	virtual void handleResponse(std::istream & inputstream) const;

private:
	FacebookCpp::FbSharedCallback<ResponseType> mSuccessCallback;
};


} // namespace FacebookCpp



#endif // __FacebookCpp_users_getInfo__