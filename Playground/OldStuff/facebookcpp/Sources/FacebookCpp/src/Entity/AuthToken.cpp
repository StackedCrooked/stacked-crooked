#include "FacebookCpp/Entity/AuthToken.h"


using namespace FacebookCpp;
using namespace std;


AuthToken::AuthToken(const string & inToken):
	mToken(inToken)
{
}


const string & AuthToken::token() const
{
	return mToken;
}