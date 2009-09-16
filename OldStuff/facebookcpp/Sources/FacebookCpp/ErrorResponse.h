#ifndef __FacebookCpp_ErrorResponse_h__
#define __FacebookCpp_ErrorResponse_h__


#include "FacebookCpp/Defines.h"
#include "XSD/facebook.hxx"
#include <string>


namespace FacebookCpp
{


class ErrorResponse
{
public:
	static ErrorResponse makeErrorResponseFromKeyValuePairs(const KeyValue & inKeyValue);

	ErrorResponse(const XSD::FacebookApiException & inFacebookApiException);

	ErrorResponse(int inErrorCode, const std::string & inErrorMessage, const KeyValue & inArgs);

	int errorCode() const;

	const std::string & errorMessage() const;

	const KeyValue & args() const;

private:
	int mErrorCode;
	std::string mErrorMessage;
	KeyValue mArgs;
};


}


#endif // __FacebookCpp_ErrorResponse_h__