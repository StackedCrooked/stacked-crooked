#include "FacebookCpp/Entity/UserInfo.h"


using namespace FacebookCpp;
using namespace std;


void UserInfo::setUid(const string & inUid)
{
	mUid = inUid;
}


void UserInfo::setAboutMe(const string & inAboutMe)
{
	mAboutMe = inAboutMe;
}


void UserInfo::setActivities(const string & inActivities)
{
	mActivities = inActivities;
}


void UserInfo::setAffiliations(const vector<Affiliation> & inAffiliations)
{
	mAffiliations = inAffiliations;
}


void UserInfo::setBirthday(const string & inBirthday)
{
	mBirthday = inBirthday;
}


void UserInfo::setBooks(const string & inBooks)
{
	mBooks = inBooks;
}


void UserInfo::setCurrentLocation(const Location & inCurrentLocation)
{
	mCurrentLocation = inCurrentLocation;
}


void UserInfo::setEducationInfoList(const vector<EducationInfo> & inEducationInfoList)
{
	mEducationInfoList = inEducationInfoList;
}


void UserInfo::setFirstName(const string & inFirstName)
{
	mFirstName = inFirstName;
}


void UserInfo::setHomeTownLocation(const Location & inHomeTownLocation)
{
	mHomeTownLocation = inHomeTownLocation;
}


void UserInfo::setHighSchoolInfo(const HighschoolInfo & inHighSchoolInfo)
{
	mHighSchoolInfo = inHighSchoolInfo;
}


void UserInfo::setIsAppUser(bool inIsAppUser)
{
	mIsAppUser = inIsAppUser;
}


void UserInfo::setHasAddedApp(bool inHasAddedApp)
{
	mHasAddedApp = inHasAddedApp;
}


void UserInfo::setInterests(const string & inInterests)
{
	mInterests = inInterests;
}


void UserInfo::setLastName(const string & inLastName)
{
	mLastName = inLastName;
}


void UserInfo::setMeetingForList(const vector<string> & inMeetingForList)
{
	mMeetingForList = inMeetingForList;
}


void UserInfo::setMeetingSexList(const vector<string> & inMeetingSexList)
{
	mMeetingSexList = inMeetingSexList;
}


void UserInfo::setMovies(const string & inMovies)
{
	mMovies = inMovies;
}


void UserInfo::setMusic(const string & inMusic)
{
	mMusic = inMusic;
}


void UserInfo::setName(const string & inName)
{
	mName = inName;
}


void UserInfo::setNotesCount(int inNotesCount)
{
	mNotesCount = inNotesCount;
}


void UserInfo::setPicUrl(const string & inPicUrl)
{
	mPicUrl = inPicUrl;
}


void UserInfo::setPicBigUrl(const string & inPicBigUrl)
{
	mPicBigUrl = inPicBigUrl;
}


void UserInfo::setPicSmallUrl(const string & inPicSmallUrl)
{
	mPicSmallUrl = inPicSmallUrl;
}


void UserInfo::setPicSquareUrl(const string & inPicSquareUrl)
{
	mPicSquareUrl = inPicSquareUrl;
}


void UserInfo::setPolitical(const string & inPolitical)
{
	mPolitical = inPolitical;
}


void UserInfo::setProfileUpdateTime(const string & inProfileUpdateTime)
{
	mProfileUpdateTime = inProfileUpdateTime;
}


void UserInfo::setQuotes(const string & inQuotes)
{
	mQuotes = inQuotes;
}


void UserInfo::setRelationshipStatus(const std::string & inRelationshipStatus)
{
	mRelationshipStatus = inRelationshipStatus;
}


const std::string & UserInfo::relationshipStatus() const
{
	return mRelationshipStatus;
}


void UserInfo::setReligion(const string & inReligion)
{
	mReligion = inReligion;
}


void UserInfo::setSex(const string & inSex)
{
	mSex = inSex;
}


void UserInfo::setSignificantOtherId(const string & inSignificantOtherId)
{
	mSignificantOtherId = inSignificantOtherId;
}


void UserInfo::setStatus(const Status & inStatus)
{
	mStatus = inStatus;
}


void UserInfo::setTimeZone(int inTimeZone)
{
	mTimeZone = inTimeZone;
}


void UserInfo::setTv(const string & inTv)
{
	mTv = inTv;
}


void UserInfo::setWallCount(int inWallCount)
{
	mWallCount = inWallCount;
}


void UserInfo::setWorkInfoList(const vector<WorkInfo> & inWorkInfoList)
{
	mWorkInfoList = inWorkInfoList;
}


const string & UserInfo::uid() const
{
	return mUid;
}


const string & UserInfo::aboutMe() const
{
	return mAboutMe;
}


const string & UserInfo::activities() const
{
	return mActivities;
}


const vector<UserInfo::Affiliation> & UserInfo::affiliations() const
{
	return mAffiliations;
}


const string & UserInfo::birthday() const
{
	return mBirthday;
}


const string & UserInfo::books() const
{
	return mBooks;
}


const UserInfo::Location & UserInfo::currentLocation() const
{
	return mCurrentLocation;
}


const vector<UserInfo::EducationInfo> & UserInfo::educationInfoList() const
{
	return mEducationInfoList;
}


const string & UserInfo::firstName() const
{
	return mFirstName;
}


const UserInfo::Location & UserInfo::homeTownLocation() const
{
	return mHomeTownLocation;
}


const UserInfo::HighschoolInfo & UserInfo::highSchoolInfo() const
{
	return mHighSchoolInfo;
}


bool UserInfo::isAppUser() const
{
	return mIsAppUser;
}


bool UserInfo::hasAddedApp() const
{
	return mHasAddedApp;
}


const string & UserInfo::interests() const
{
	return mInterests;
}


const string & UserInfo::lastName() const
{
	return mLastName;
}


const vector<string> & UserInfo::meetingForList() const
{
	return mMeetingForList;
}


const vector<string> & UserInfo::meetingSexList() const
{
	return mMeetingSexList;
}


const string & UserInfo::movies() const
{
	return mMovies;
}


const string & UserInfo::music() const
{
	return mMusic;
}


const string & UserInfo::name() const
{
	return mName;
}


int UserInfo::notesCount() const
{
	return mNotesCount;
}


const string & UserInfo::picUrl() const
{
	return mPicUrl;
}


const string & UserInfo::picBigUrl() const
{
	return mPicBigUrl;
}


const string & UserInfo::picSmallUrl() const
{
	return mPicSmallUrl;
}


const string & UserInfo::picSquareUrl() const
{
	return mPicSquareUrl;
}


const string & UserInfo::political() const
{
	return mPolitical;
}


const string & UserInfo::profileUpdateTime() const
{
	return mProfileUpdateTime;
}


const string & UserInfo::quotes() const
{
	return mQuotes;
}


const string & UserInfo::religion() const
{
	return mReligion;
}


const string & UserInfo::sex() const
{
	return mSex;
}


const string & UserInfo::significantOtherId() const
{
	return mSignificantOtherId;
}


const UserInfo::Status & UserInfo::status() const
{
	return mStatus;
}


int UserInfo::timeZone() const
{
	return mTimeZone;
}


const string & UserInfo::tv() const
{
	return mTv;
}


int UserInfo::wallCount() const
{
	return mWallCount;
}


const vector<UserInfo::WorkInfo> & UserInfo::workInfoList() const
{
	return mWorkInfoList;
}

