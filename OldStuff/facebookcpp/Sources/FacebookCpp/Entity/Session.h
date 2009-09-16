#ifndef __FacebookCpp_Session_h__
#define __FacebookCpp_Session_h__


#include <string>


namespace FacebookCpp
{


class Session
{
public:

	Session(){}
		/// Creates an invalid session

	Session(const std::string & inSessionKey, const std::string & inUID, const std::string & inExpires, const std::string & inSecret);
	
	const std::string & sessionKey() const { return mSessionKey; }
	
	const std::string & uid() const { return mUID; }
	
	const std::string & expires() const { return mExpires; }
	
	const std::string & secret() const { return mSecret; }

private:
	std::string mSessionKey;
	std::string mUID;
	std::string mExpires;
	std::string mSecret;
};


}


#endif // __FacebookCpp_Session_h__