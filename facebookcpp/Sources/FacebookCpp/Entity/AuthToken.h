#ifndef __FacebookCpp_AuthToken_h__
#define __FacebookCpp_AuthToken_h__


#include <string>


namespace FacebookCpp
{


class AuthToken
{
public:
	AuthToken(const std::string & inToken);

	const std::string & token() const;

private:
	std::string mToken;
};


}


#endif // __FacebookCpp_AuthToken_h__