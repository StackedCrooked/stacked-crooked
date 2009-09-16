#ifndef __FacebookCpp_UserInfo_h__
#define __FacebookCpp_UserInfo_h__


#include <map>
#include <string>
#include <vector>


namespace FacebookCpp
{


// http://developers.facebook.com/documentation.php?v=1.0&method=Users.getInfo

//<?xml version="1.0" encoding="UTF-8"?>
//<users_getInfo_response xmlns="http://api.facebook.com/1.0/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://api.facebook.com/1.0/ http://api.facebook.com/1.0/facebook.xsd" list="true">
//	<user>
//		<uid>8055</uid>
//		<about_me>This field perpetuates the glorification of the ego.	Also, it has a character limit.</about_me>
//		<activities>Here: facebook, etc. There: Glee Club, a capella, teaching.</activities>
//		<affiliations list="true">
//			<affiliation>
//				<nid>50453093</nid>
//				<name>Facebook Developers</name>
//				<type>work</type>
//				<status/>
//				<year/>
//			</affiliation>
//		</affiliations>
//		<birthday>November 3</birthday>
//		<books>The Brothers K, GEB, Ken Wilber, Zen and the Art, Fitzgerald, The Emporer's New Mind, The Wonderful Story of Henry Sugar</books>
//		<current_location>
//			<city>Palo Alto</city>
//			<state>CA</state>
//			<country>United States</country>
//			<zip>94303</zip>
//		</current_location>
//		<education_history list="true">
//			<education_info>
//				<name>Harvard</name>
//				<year>2003</year>
//				<concentrations list="true">
//					<concentration>Applied Mathematics</concentration>
//					<concentration>Computer Science</concentration>
//				</concentrations>
//				<degree>MS</degree>
//			</education_info>
//		</education_history>
//		<first_name>Dave</first_name>
//		 <hometown_location>
//			 <city>York</city>
//			 <state>PA</state>
//			 <country>United States</country>
//			 <zip>0</zip>
//		 </hometown_location>
//		 <hs_info>
//			 <hs1_name>Central York High School</hs1_name>
//			 <hs2_name/>
//			 <grad_year>1999</grad_year>
//			 <hs1_id>21846</hs1_id>
//			 <hs2_id>0</hs2_id>
//		 </hs_info>
//		 <is_app_user>1</is_app_user>
//		 <has_added_app>1</has_added_app>
//		 <interests>coffee, computers, the funny, architecture, code breaking,snowboarding, philosophy, soccer, talking to strangers</interests>
//		 <last_name>Fetterman</last_name>
//		 <meeting_for list="true">
//			 <seeking>Friendship</seeking>
//		 </meeting_for>
//		 <meeting_sex list="true">
//			 <sex>female</sex>
//		 </meeting_sex>
//		 <movies>Tommy Boy, Billy Madison, Fight Club, Dirty Work, Meet the Parents, My Blue Heaven, Office Space </movies>
//		 <music>New Found Glory, Daft Punk, Weezer, The Crystal Method, Rage, the KLF, Green Day, Live, Coldplay, Panic at the Disco, Family Force 5</music>
//		 <name>Dave Fetterman</name>
//		 <notes_count>0</notes_count>
//		 <pic>http://photos-055.facebook.com/ip007/profile3/1271/65/s8055_39735.jpg</pic>
//		 <pic_big>http://photos-055.facebook.com/ip007/profile3/1271/65/n8055_39735.jpg</pic>
//		 <pic_small>http://photos-055.facebook.com/ip007/profile3/1271/65/t8055_39735.jpg</pic>
//		 <pic_square>http://photos-055.facebook.com/ip007/profile3/1271/65/q8055_39735.jpg</pic>
//		 <political>Moderate</political>
//		 <profile_update_time>1170414620</profile_update_time>
//		 <quotes/>
//		 <relationship_status>In a Relationship</relationship_status>
//		 <religion/>
//		 <sex>male</sex>
//		 <significant_other_id xsi:nil="true"/>
//		 <status>
//			 <message/>
//			 <time>0</time>
//		 </status>
//		 <timezone>-8</timezone>
//		 <tv>cf. Bob Trahan</tv>
//		 <wall_count>121</wall_count>
//		 <work_history list="true">
//			 <work_info>
//				 <location>
//					 <city>Palo Alto</city>
//					 <state>CA</state>
//					 <country>United States</country>
//				 </location>
//				 <company_name>Facebook</company_name>
//				 <position>Software Engineer</position>
//				 <description>Tech Lead, Facebook Platform</description>
//				 <start_date>2006-01</start_date>
//				 <end_date/>
//				</work_info>
//		 </work_history>
//	 </user>
//</users_getInfo_response>


class UserInfo
{
public:
	struct Affiliation
	{
		//	<affiliation>
		//		<nid>50453093</nid>
		//		<name>Facebook Developers</name>
		//		<type>work</type>
		//		<status/>
		//		<year/>
		//	</affiliation>
		std::string Nid;
		std::string Name;
		std::string Type;
		std::string Status;
		std::string Year;
	};

	struct Location
	{
		//	<hometown_location>
		//		<city>York</city>
		//		<state>PA</state>
		//		<country>United States</country>
		//		<zip>0</zip>
		//	</hometown_location>
		std::string City;
		std::string State;
		std::string Country;
		std::string Zip;
	};

	typedef Location HomeTownLocation;
		//	<hometown_location>
		//		<city>York</city>
		//		<state>PA</state>
		//		<country>United States</country>
		//		<zip>0</zip>
		//	</hometown_location>

	typedef Location CurrentLocation;
		//	<current_location>
		//		<city>Palo Alto</city>
		//		<state>CA</state>
		//		<country>United States</country>
		//		<zip>94303</zip>
		//	</current_location>
	
	struct EducationInfo
	{
		//	<education_info>
		//		<name>Harvard</name>
		//		<year>2003</year>
		//		<concentrations list="true">
		//			<concentration>Applied Mathematics</concentration>
		//			<concentration>Computer Science</concentration>
		//		</concentrations>
		//		<degree>MS</degree>
		//	</education_info>
		std::string School;
		std::string Year;
		std::vector<std::string> Concentrations;
		std::string Degree;
	};

	struct HighschoolInfo
	{
		//	<hs_info>
		//		<hs1_name>Central York High School</hs1_name>
		//		<hs2_name/>
		//		<grad_year>1999</grad_year>
		//		<hs1_id>21846</hs1_id>
		//		<hs2_id>0</hs2_id>
		//	</hs_info>
		std::string HsName; 
		std::string Hs2Name; 
		std::string GradYear; 
		std::string Hs1Id; 
		std::string Hs2Id; 
	};

	struct Status
	{
		//	<status>
		//		<message/>
		//		<time>0</time>
		//	</status>
		std::string Message;
		std::string Time;
	};

	struct WorkInfo
	{
			//	<work_info>
			//		<location>
			//			<city>Palo Alto</city>
			//			<state>CA</state>
			//			<country>United States</country>
			//		</location>
			//		<company_name>Facebook</company_name>
			//		<position>Software Engineer</position>
			//		<description>Tech Lead, Facebook Platform</description>
			//		<start_date>2006-01</start_date>
			//		<end_date/>
			//	</work_info>
			Location Location;
			std::string CompanyName;
			std::string Position;
			std::string Description;
			std::string StartDate;
			std::string EndDate;
	};

	UserInfo(){}

	const std::string & uid() const;

	void setUid(const std::string & inUid);

	const std::string & aboutMe() const;

	void setAboutMe(const std::string & inAboutMe);

	const std::string & activities() const;

	void setActivities(const std::string & inActivities);

	const std::vector<Affiliation> & affiliations() const;

	void setAffiliations(const std::vector<Affiliation> & inAffiliations);

	const std::string & birthday() const;

	void setBirthday(const std::string & inBirthday);

	const std::string & books() const;

	void setBooks(const std::string & inBooks);

	const Location & currentLocation() const;

	void setCurrentLocation(const Location & inCurrentLocation);

	const std::vector<EducationInfo> & educationInfoList() const;

	void setEducationInfoList(const std::vector<EducationInfo> & inEducationInfoList);

	const std::string & firstName() const;

	void setFirstName(const std::string & inFirstName);

	const Location & homeTownLocation() const;

	void setHomeTownLocation(const Location & inHomeTownLocation);

	const HighschoolInfo & highSchoolInfo() const;

	void setHighSchoolInfo(const HighschoolInfo & inHighSchoolInfo);

	bool isAppUser() const;

	void setIsAppUser(bool inIsAppUser);

	bool hasAddedApp() const;

	void setHasAddedApp(bool inHasAddedApp);

	const std::string & interests() const;

	void setInterests(const std::string & inInterests);

	const std::string & lastName() const;

	void setLastName(const std::string & inLastName);

	const std::vector<std::string> & meetingForList() const;

	void setMeetingForList(const std::vector<std::string> & inMeetingForList);

	const std::vector<std::string> & meetingSexList() const;

	void setMeetingSexList(const std::vector<std::string> & inMeetingSexList);

	const std::string & movies() const;

	void setMovies(const std::string & inMovies);

	const std::string & music() const;

	void setMusic(const std::string & inMusic);

	const std::string & name() const;

	void setName(const std::string & inName);

	int notesCount() const;

	void setNotesCount(int inNotesCount);

	const std::string & picUrl() const;

	void setPicUrl(const std::string & inPicUrl);

	const std::string & picBigUrl() const;

	void setPicBigUrl(const std::string & inPicBigUrl);

	const std::string & picSmallUrl() const;

	void setPicSmallUrl(const std::string & inPicSmallUrl);

	const std::string & picSquareUrl() const;

	void setPicSquareUrl(const std::string & inPicSquareUrl);

	const std::string & political() const;

	void setPolitical(const std::string & inPolitical);

	const std::string & profileUpdateTime() const;

	void setProfileUpdateTime(const std::string & inProfileUpdateTime);

	const std::string & quotes() const;

	void setRelationshipStatus(const std::string & inRelationshipStatus);

	const std::string & relationshipStatus() const;

	void setQuotes(const std::string & inQuotes);

	const std::string & religion() const;

	void setReligion(const std::string & inReligion);

	const std::string & sex() const;

	void setSex(const std::string & inSex);

	const std::string & significantOtherId() const;

	void setSignificantOtherId(const std::string & inSignificantOtherId);

	const Status & status() const;

	void setStatus(const Status & inStatus);

	int timeZone() const;

	void setTimeZone(int inTimeZone);

	const std::string & tv() const;

	void setTv(const std::string & inTv);

	int wallCount() const;

	void setWallCount(int inWallCount);

	const std::vector<WorkInfo> & workInfoList() const;

	void setWorkInfoList(const std::vector<WorkInfo> & inWorkInfoList);


private:
	std::string mUid;
		//	<uid>8055</uid>
	
	std::string mAboutMe;
		//	<about_me>This field perpetuates the glorification of the ego.</about_me>
	
	std::string mActivities;
		//	<activities>Here: facebook, etc. There: Glee Club, a capella, teaching.</activities>
	
	std::vector<Affiliation> mAffiliations;
		//	<affiliations list="true">
		//		<affiliation>
		//			<nid>50453093</nid>
		//			<name>Facebook Developers</name>
		//			<type>work</type>
		//			<status/>
		//			<year/>
		//		</affiliation>
		//	</affiliations>
	
	std::string mBirthday;
		//	<birthday>November 3</birthday>

	std::string mBooks;
		//	<books>The Brothers K, GEB, Ken Wilber</books>
	
	Location mCurrentLocation;
		//	<current_location>

	std::vector<EducationInfo> mEducationInfoList;
		//	<education_history list="true">
		//		<education_info>
		//			<name>Harvard</name>
		//			<year>2003</year>
		//			<concentrations list="true">
		//				<concentration>Applied Mathematics</concentration>
		//				<concentration>Computer Science</concentration>
		//			</concentrations>
		//			<degree>MS</degree>
		//		</education_info>
		//	</education_history>

	std::string mFirstName;
		// <first_name>Dave</first_name>

	Location mHomeTownLocation;
		// <hometown_location>

	HighschoolInfo mHighSchoolInfo;
		//	<hs_info>

	bool mIsAppUser;
		// <is_app_user>1</is_app_user>

	bool mHasAddedApp;
		// <has_added_app>1</has_added_app>

	std::string mInterests;
		// <interests>coffee, computers, the funny, architecture, code breaking,snowboarding, philosophy, soccer, talking to strangers</interests>

	std::string mLastName;
		// <last_name>Fetterman</last_name>
	
	std::vector<std::string> mMeetingForList;
		//	<meeting_for list="true">
		//		<seeking>Friendship</seeking>
		//	</meeting_for>

	std::vector<std::string> mMeetingSexList;
		//	<meeting_sex list="true">
		//		<sex>female</sex>
		//	</meeting_sex>

	std::string mMovies;
		// <movies>Tommy Boy, Billy Madison, Fight Club</movies>

	std::string mMusic;
		// <music>New Found Glory, Daft Punk, Weezer</music>

	std::string mName;
		// <name>Dave Fetterman</name>

	int mNotesCount;
		// <notes_count>0</notes_count>

	std::string mPicUrl;
		// <pic>http://photos-055.facebook.com/ip007/profile3/1271/65/s8055_39735.jpg</pic>

	std::string mPicBigUrl;
		// <pic_big>http://photos-055.facebook.com/ip007/profile3/1271/65/n8055_39735.jpg</pic>

	std::string mPicSmallUrl;
		// <pic_small>http://photos-055.facebook.com/ip007/profile3/1271/65/t8055_39735.jpg</pic>

	std::string mPicSquareUrl;
		// <pic_square>http://photos-055.facebook.com/ip007/profile3/1271/65/q8055_39735.jpg</pic>

	std::string mPolitical;
		// <political>Moderate</political>
	
	std::string mProfileUpdateTime;
		// <profile_update_time>1170414620</profile_update_time>

	std::string mQuotes;
		// <quotes/>

	std::string mRelationshipStatus;
		// <relationship_status>In a Relationship</relationship_status>

	std::string mReligion;
		// <religion/>

	std::string mSex;
		// <sex>male</sex>

	std::string mSignificantOtherId;
		// <significant_other_id xsi:nil="true"/>

	Status mStatus;
		//	<status>
		//		<message/>
		//		<time>0</time>
		//	</status>

	int mTimeZone;
		// <timezone>-8</timezone>

	std::string mTv;
		// <tv>cf. Bob Trahan</tv>

	int mWallCount;
		// <wall_count>121</wall_count>

	std::vector<WorkInfo> mWorkInfoList;
		//	<work_history list="true">
		//		<work_info>
		//			<location>
		//				<city>Palo Alto</city>
		//				<state>CA</state>
		//				<country>United States</country>
		//			</location>
		//			<company_name>Facebook</company_name>
		//			<position>Software Engineer</position>
		//			<description>Tech Lead, Facebook Platform</description>
		//			<start_date>2006-01</start_date>
		//			<end_date/>
		//		</work_info>
		//	</work_history>
};


} // namespace FacebookCpp


#endif // __FacebookCpp_UserInfo_h__


