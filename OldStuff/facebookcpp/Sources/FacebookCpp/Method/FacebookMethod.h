#ifndef __FacebookCpp_FacebookMethod_h__
#define __FacebookCpp_FacebookMethod_h__


#include "FacebookCpp/Defines.h"
#include "FacebookCpp/ErrorResponse.h"
#include "FacebookCpp/FacebookUtil.h"
#include "Poco/Delegate.h"


namespace FacebookCpp
{


class FacebookMethod
{
public:

	FacebookMethod(const std::string & inAPIMethod, const Poco::AbstractDelegate<ErrorResponse> & inErrorCallback);
	
	virtual ~FacebookMethod();
	
	typedef std::map<std::string,std::string> Params;

	Params getParams() const;

	virtual void handleResponse(std::istream & inputstream) const = 0;

protected:
	Params mParams;
	FacebookCpp::FbSharedCallback<ErrorResponse> mErrorCallback;

private:
	std::string mAPIMethod;
};


}


#endif // __FacebookCpp_FacebookMethod_h__