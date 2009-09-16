#include "FacebookCpp/Entity/FriendInfo.h"


using namespace FacebookCpp;
using namespace std;


FriendInfo::FriendInfo(const string & inUid1, const string & inUid2, bool inAreFriends):
	mUid1(inUid1),
	mUid2(inUid2),
	mAreFriends(inAreFriends)
{
}


const string FriendInfo::uid1() const
{
	return mUid1;
}


const string FriendInfo::uid2() const
{
	return mUid2;
}


bool FriendInfo::areFriends() const
{
	return mAreFriends;
}