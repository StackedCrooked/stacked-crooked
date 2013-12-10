#ifndef __FacebookCpp_friends_areFriends_h__
#define __FacebookCpp_friends_areFriends_h__


#include "FacebookCpp/Method/FacebookSessionMethod.h"
#include "XSD/facebook.hxx"


namespace FacebookCpp
{


class friends_areFriends :	public FacebookSessionMethod
{
public:

	typedef std::auto_ptr<XSD::friends_areFriends_response> ResponseType;

	friends_areFriends
	(
		const Session & inSession,
		const std::vector<std::string> & inUids1,
		const std::vector<std::string> & inUids2,
		const Poco::AbstractDelegate<ResponseType> & inSuccessCallback,
		const Poco::AbstractDelegate<ErrorResponse> & inErrorCallback
	);

	virtual void handleResponse(std::istream & inputstream) const;

private:
	FacebookCpp::FbSharedCallback<ResponseType> mSuccessCallback;
};


} // namespace FacebookCpp


#endif // __FacebookCpp_friends_areFriends_h__