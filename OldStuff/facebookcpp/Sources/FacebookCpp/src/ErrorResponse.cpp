#include "FacebookCpp/ErrorResponse.h"


#include <sstream>


using namespace FacebookCpp;
using namespace FacebookCpp::XSD;
using namespace std;


ErrorResponse::ErrorResponse(const FacebookApiException & inFacebookApiException)
{
	// error_code
	mErrorCode = inFacebookApiException.error_code();

	// error_msg
	mErrorMessage = inFacebookApiException.error_msg();

	// request_args
	const request_args::arg_sequence & args = inFacebookApiException.request_args().arg();
	request_args::arg_sequence::const_iterator it = args.begin(), end = args.end();
	for(; it != end; ++it)
	{
		mArgs[it->key()] = it->value();
	}
}


ErrorResponse::ErrorResponse(int inErrorCode, const string & inErrorMessage, const KeyValue & inArgs):
	mErrorCode(inErrorCode),
	mErrorMessage(inErrorMessage),
	mArgs(inArgs)
{
}


int ErrorResponse::errorCode() const
{
	return mErrorCode;
}


const string & ErrorResponse::errorMessage() const
{
	return mErrorMessage;
}


const KeyValue & ErrorResponse::args() const
{
	return mArgs;
}

ErrorResponse ErrorResponse::makeErrorResponseFromKeyValuePairs(const KeyValue & inKeyValue)
{
	int errorCode = 0;
	string errorCodeString;
	KeyValue::const_iterator it = inKeyValue.find("error_code");
	if(it != inKeyValue.end())
	{
		errorCodeString = it->second;
		istringstream errorCodeStream(errorCodeString);
		errorCodeStream >> errorCode;
	}

	KeyValue::const_iterator it_message = inKeyValue.find("error_msg");
	string errorMessage;
	if(it_message != inKeyValue.end())
	{
		errorMessage = it_message->second;
	}
	
	return ErrorResponse(errorCode, errorMessage, inKeyValue);
}