#ifndef __FacebookCpp_FriendInfo_h__
#define __FacebookCpp_FriendInfo_h__


#include <string>


namespace FacebookCpp
{


class FriendInfo
{
public:
	FriendInfo(const std::string & inUid1, const std::string & inUid2, bool inAreFriends);

	const std::string uid1() const;

	const std::string uid2() const;

	bool areFriends() const;

private:
	std::string mUid1;
	std::string mUid2;
	bool mAreFriends;
};


}


#endif // __FacebookCpp_FriendInfo_h__