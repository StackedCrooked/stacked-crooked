#ifndef __FacebookCpp_FacebookSessionMethod_h__
#define __FacebookCpp_FacebookSessionMethod_h__


#include "FacebookCpp/Method/FacebookMethod.h"
#include "FacebookCpp/Entity/Session.h"


namespace FacebookCpp
{


class FacebookSessionMethod : public FacebookMethod
{
public:
	FacebookSessionMethod(const Session & inSession, const std::string & inAPIMethod, const Poco::AbstractDelegate<ErrorResponse> & inErrorCallback);

	virtual ~FacebookSessionMethod() = 0{}

private:
	Session mSession;
};


}


#endif // __FacebookCpp_FacebookSessionMethod_h__