#include "FacebookCpp/Entity/Session.h"


using namespace FacebookCpp;
using namespace std;


Session::Session(const string & inSessionKey, const string & inUID, const string & inExpires, const string & inSecret):
	mSessionKey(inSessionKey),
	mUID(inUID),
	mExpires(inExpires),
	mSecret(inSecret)
{
}