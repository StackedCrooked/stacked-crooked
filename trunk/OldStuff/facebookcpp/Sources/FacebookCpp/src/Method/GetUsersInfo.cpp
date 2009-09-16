#include "FacebookCpp/Method/GetUsersInfo.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/SAX/SAXParser.h"
#include "Poco/String.h"
#include <iostream>


using namespace FacebookCpp;
using namespace Poco;
using namespace XML;
using namespace std;



std::map<std::string, GetUsersInfo::FbStruct> GetUsersInfo::sMapping_ElementName_FbStruct;


GetUsersInfo::GetUsersInfo
(
	const Session & inSession,
	const vector<string> & inUids,
	const AbstractDelegate<ResponseType> & inSuccessCallback,
	const AbstractDelegate<ErrorResponse> & inErrorCallback
):
	FacebookSessionMethod(inSession, "Users.getInfo", inErrorCallback),
	mUids(inUids),
	mCurrentStruct(eNone),
	mSuccessCallback(inSuccessCallback)
{
	mParams["uids"] = toCSV(inUids);
	mParams["fields"] += "about_me";
	mParams["fields"] += ",activities";
	mParams["fields"] += ",affiliations";
	mParams["fields"] += ",birthday";
	mParams["fields"] += ",books";
	mParams["fields"] += ",current_location";
	mParams["fields"] += ",education_history";
	mParams["fields"] += ",first_name";
	mParams["fields"] += ",has_added_app";
	mParams["fields"] += ",hometown_location";
	mParams["fields"] += ",hs_info";
	mParams["fields"] += ",interests";
	mParams["fields"] += ",is_app_user";
	mParams["fields"] += ",last_name";
	mParams["fields"] += ",meeting_for";
	mParams["fields"] += ",meeting_sex";
	mParams["fields"] += ",movies";
	mParams["fields"] += ",music";
	mParams["fields"] += ",name";
	mParams["fields"] += ",notes_count";
	mParams["fields"] += ",pic";
	mParams["fields"] += ",pic_big";
	mParams["fields"] += ",pic_small";
	mParams["fields"] += ",pic_square";
	mParams["fields"] += ",political";
	mParams["fields"] += ",profile_update_time";
	mParams["fields"] += ",quotes";
	mParams["fields"] += ",relationship_status";
	mParams["fields"] += ",religion";
	mParams["fields"] += ",sex";
	mParams["fields"] += ",significant_other_id";
	mParams["fields"] += ",status";
	mParams["fields"] += ",timezone";
	mParams["fields"] += ",tv";
	mParams["fields"] += ",uid";
	mParams["fields"] += ",wall_count";
	mParams["fields"] += ",work_history";
}


void GetUsersInfo::setDocumentLocator(const Locator* loc)
{
}


void GetUsersInfo::startDocument()
{
}


void GetUsersInfo::endDocument()
{
}


void GetUsersInfo::startElement(const XMLString& uri, const XMLString& localName, const XMLString& qname, const Attributes& attrList)
{
	if(sMapping_ElementName_FbStruct.empty())
	{
		sMapping_ElementName_FbStruct["affiliations"] = eAffiliationList;
		sMapping_ElementName_FbStruct["current_location"] = eCurrentLocation;
		sMapping_ElementName_FbStruct["education_history"] = eEducationHistory;
		sMapping_ElementName_FbStruct["hometown_location"] = eHomeTownLocation;
		sMapping_ElementName_FbStruct["hs_info"] = eHighschoolInfo;
		sMapping_ElementName_FbStruct["meeting_for"] = eMeetingFor;
		sMapping_ElementName_FbStruct["meeting_sex"] = eMeetingSex;
		sMapping_ElementName_FbStruct["status"] = eStatus;
		sMapping_ElementName_FbStruct["work_history"] = eWorkHistory;
	}
	if(mCurrentStruct == eNone)
	{
		if(sMapping_ElementName_FbStruct.find(localName) != sMapping_ElementName_FbStruct.end())
		{
			mCurrentStruct = sMapping_ElementName_FbStruct[localName];
		}
	}
	else
	{
		switch(mCurrentStruct)
		{
			case eAffiliationList:
			{
				//	<affiliations list="true">
				//		<affiliation>
				//			<nid>50453093</nid>
				//			<name>Facebook Developers</name>
				//			<type>work</type>
				//			<status/>
				//			<year/>
				//		</affiliation>
				//	</affiliations>
				if(localName == "affiliation")
				{
					mCurrentStruct = eAffiliationList_Affiliation;
				}
				else
				{
					mCurrentStruct = eNone;
				}
				break;
			}
			case eEducationHistory:
			{
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
				if(localName == "education_info")
				{
					mCurrentStruct = eEducationHistory_EducationInfo;
				}
				else
				{
					mCurrentStruct = eNone;
				}
				break;
			}
			case eEducationHistory_EducationInfo:
			{
				if(localName == "concentrations")
				{
					mCurrentStruct = eEducationHistory_EducationInfo_Concentrations;
				}
				else
				{
					mCurrentStruct = eEducationHistory;
				}
				break;
			}
			case eWorkHistory:
			{
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
				if(localName == "work_info")
				{
					mCurrentStruct = eWorkHistory_WorkInfo;
				}
				else
				{
					mCurrentStruct = eNone;
				}
				break;
			}
			case eWorkHistory_WorkInfo:
			{
				if(localName == "location")
				{
					mCurrentStruct = eWorkHistory_WorkInfo_Location;
				}
				else
				{
					mCurrentStruct = eWorkHistory;
				}
				break;
			}
			default:
			{
				mCurrentStruct = eNone;
				break;
			}
		};
	}
	mLocalName = localName;
}


void GetUsersInfo::endElement(const XMLString& uri, const XMLString& localName, const XMLString& qname)
{
	switch(mCurrentStruct)
	{
		case eNone:
		{
			if(localName == "user")
			{
				mUserInfoList.push_back(mUserInfo);
				mUserInfo = UserInfo();
			}
			else if(localName == "about_me")
			{
				mUserInfo.setAboutMe(mCurrentString);
			}
			else if(localName == "activities")
			{
				mUserInfo.setActivities(mCurrentString);
			}
			else if(localName == "birthday")
			{
				mUserInfo.setBirthday(mCurrentString);
			}
			else if(localName == "books")
			{
				mUserInfo.setBooks(mCurrentString);
			}
			else if(localName == "first_name")
			{
				mUserInfo.setFirstName(mCurrentString);
			}
			else if(localName == "has_added_app")
			{
				mUserInfo.setHasAddedApp(trim(mCurrentString) == "1");
			}
			else if(localName == "interests")
			{
				mUserInfo.setInterests(mCurrentString);
			}
			else if(localName == "is_app_user")
			{
				mUserInfo.setIsAppUser(trim(mCurrentString) == "1");
			}
			else if(localName == "last_name")
			{
				mUserInfo.setLastName(mCurrentString);
			}
			else if(localName == "movies")
			{
				mUserInfo.setMovies(mCurrentString);
			}
			else if(localName == "music")
			{
				mUserInfo.setMusic(mCurrentString);
			}
			else if(localName == "name")
			{
				mUserInfo.setName(mCurrentString);
			}
			else if(localName == "notes_count")
			{
				string notesCountString = trim(mCurrentString);
				istringstream ss(notesCountString);
				int notesCount;
				ss >> notesCount;
				mUserInfo.setNotesCount(notesCount);
			}
			else if(localName == "pic")
			{
				mUserInfo.setPicUrl(mCurrentString);
			}
			else if(localName == "pic_big")
			{
				mUserInfo.setPicBigUrl(mCurrentString);
			}
			else if(localName == "pic_small")
			{
				mUserInfo.setPicSmallUrl(mCurrentString);
			}
			else if(localName == "pic_square")
			{
				mUserInfo.setPicSquareUrl(mCurrentString);
			}
			else if(localName == "political")
			{
				mUserInfo.setPolitical(mCurrentString);
			}
			else if(localName == "profile_update_time")
			{
				mUserInfo.setProfileUpdateTime(mCurrentString);
			}
			else if(localName == "quotes")
			{
				mUserInfo.setQuotes(mCurrentString);
			}
			else if(localName == "relationship_status")
			{
				mUserInfo.setRelationshipStatus(mCurrentString);
			}
			else if(localName == "religion")
			{
				mUserInfo.setReligion(mCurrentString);
			}
			else if(localName == "sex")
			{
				mUserInfo.setSex(mCurrentString);
			}
			else if(localName == "significant_other_id")
			{
				mUserInfo.setSignificantOtherId(mCurrentString);
			}
			else if(localName == "timezone")
			{
				string timeZoneString = trim(mCurrentString);
				istringstream ss(timeZoneString);
				int timeZone;
				ss >> timeZone;
				mUserInfo.setTimeZone(timeZone);
			}
			else if(localName == "tv")
			{
				mUserInfo.setTv(mCurrentString);
			}
			else if(localName == "uid")
			{
				mUserInfo.setUid(mCurrentString);
			}
			else if(localName == "wall_count")
			{
				string wallCountString = trim(mCurrentString);
				istringstream ss(wallCountString);
				int wallCount;
				ss >> wallCount;
				mUserInfo.setWallCount(wallCount);
			}
			break;
		}
		case eAffiliationList:
		{
			mUserInfo.setAffiliations(mAffiliations);
			mCurrentStruct = eNone;
			break;
		}
		case eAffiliationList_Affiliation:
		{
			mAffiliations.push_back(mAffiliation);
			mAffiliation = UserInfo::Affiliation();
			mCurrentStruct = eAffiliationList;
			break;
		}
		case eHomeTownLocation:
		{
			mUserInfo.setHomeTownLocation(mHomeTownLocation);
			mHomeTownLocation = UserInfo::HomeTownLocation();
			mCurrentStruct = eNone;
			break;
		}
		case eCurrentLocation:
		{
			mUserInfo.setCurrentLocation(mCurrentLocation);
			mCurrentLocation = UserInfo::Location();
			mCurrentStruct = eNone;
			break;
		}
		case eEducationHistory:
		{
			mUserInfo.setEducationInfoList(mEducationHistory);
			mEducationHistory.clear();
			mCurrentStruct = eNone;
			break;
		}
		case eEducationHistory_EducationInfo:
		{
			mEducationHistory.push_back(mEducationInfo);
			mEducationInfo = UserInfo::EducationInfo();
			mCurrentStruct = eEducationHistory;
			break;
		}
		case eEducationHistory_EducationInfo_Concentrations:
		{
			mEducationInfo.Concentrations = mEducationConcentrations;
			mEducationConcentrations.clear();
			mCurrentStruct = eEducationHistory_EducationInfo;
			break;
		}
		case eHighschoolInfo:
		{
			mUserInfo.setHighSchoolInfo(mHighschoolInfo);
			mHighschoolInfo = UserInfo::HighschoolInfo();
			mCurrentStruct = eNone;
			break;
		}
		case eStatus:
		{
			mUserInfo.setStatus(mStatus);
			mStatus = UserInfo::Status();
			mCurrentStruct = eNone;
			break;
		}
		case eWorkHistory:
		{
			mUserInfo.setWorkInfoList(mWorkHistory);
			mWorkHistory.clear();
			mCurrentStruct = eNone;
			break;
		}
		case eWorkHistory_WorkInfo:
		{
			mWorkHistory.push_back(mWorkInfo);
			mWorkInfo = UserInfo::WorkInfo();			
			mCurrentStruct = eWorkHistory;
			break;
		}
		case eWorkHistory_WorkInfo_Location:
		{
			mWorkInfo.Location = mWorkInfoLocation;
			mWorkInfoLocation = UserInfo::Location();
			mCurrentStruct = eWorkHistory_WorkInfo;
			break;
		}
	}
	mCurrentString = "";
}


void GetUsersInfo::characters(const XMLChar ch[], int start, int length)
{
	string content(ch + start, length);
	switch(mCurrentStruct)
	{
		case eNone:
		{
			mCurrentString += content;
			break;
		}
		case eAffiliationList:
		{
			//	<affiliations list="true">
			//		<affiliation>
			//			<nid>50453093</nid>
			//			<name>Facebook Developers</name>
			//			<type>work</type>
			//			<status/>
			//			<year/>
			//		</affiliation>
			//	</affiliations>

			// do nothing...
			break;
		}
		case eAffiliationList_Affiliation:
		{	
			//	<affiliation>
			//		<nid>50453093</nid>
			//		<name>Facebook Developers</name>
			//		<type>work</type>
			//		<status/>
			//		<year/>
			//	</affiliation>
			if(mLocalName == "nid")
			{
				mAffiliation.Nid += content;
			}
			else if(mLocalName == "name")
			{
				mAffiliation.Name += content;
			}
			else if(mLocalName == "type")
			{
				mAffiliation.Type += content;
			}
			else if(mLocalName == "status")
			{
				mAffiliation.Status += content;
			}
			else if(mLocalName == "year")
			{
				mAffiliation.Year = content;
			}
			break;
		}
		case eHomeTownLocation:
		{
			//	<hometown_location>
			//		<city>York</city>
			//		<state>PA</state>
			//		<country>United States</country>
			//		<zip>0</zip>
			//	</hometown_location>
			if(mLocalName == "city")
			{
				mHomeTownLocation.City += content;
			}
			else if(mLocalName == "state")
			{
				mHomeTownLocation.State += content;
			}
			else if(mLocalName == "country")
			{
				mHomeTownLocation.Country += content;
			}
			else if(mLocalName == "zip")
			{
				mHomeTownLocation.Zip += content;
			}
			break;
		}
		case eCurrentLocation:
		{
			//	<current_location>
			//		<city>Palo Alto</city>
			//		<state>CA</state>
			//		<country>United States</country>
			//		<zip>94303</zip>
			//	</current_location>
			if(mLocalName == "city")
			{
				mCurrentLocation.City += content;
			}
			else if(mLocalName == "state")
			{
				mCurrentLocation.State += content;
			}
			else if(mLocalName == "country")
			{
				mCurrentLocation.Country += content;
			}
			else if(mLocalName == "zip")
			{
				mCurrentLocation.Zip += content;
			}
			break;
		}
		case eEducationHistory:
		{
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
			
			// no useful information...
			break;
		}
		case eEducationHistory_EducationInfo:		
		{
			//		<education_info>
			//			<name>Harvard</name>
			//			<year>2003</year>
			//			<concentrations list="true">
			//				<concentration>Applied Mathematics</concentration>
			//				<concentration>Computer Science</concentration>
			//			</concentrations>
			//			<degree>MS</degree>
			//		</education_info>
			if(mLocalName == "name")
			{
				mEducationInfo.School += content;
			}
			else if(mLocalName == "year")
			{
				mEducationInfo.Year += content;
			}
			else if(mLocalName == "degree")
			{
				mEducationInfo.Degree += content;
			}
			break;
		}
		case eEducationHistory_EducationInfo_Concentrations:		
		{
			//			<concentrations list="true">
			//				<concentration>Applied Mathematics</concentration>
			//				<concentration>Computer Science</concentration>
			//			</concentrations>
			if(mLocalName == "concentration")
			{
				mEducationConcentration += content;
			}
			break;
		}
		case eHighschoolInfo:
		{
			//	<hs_info>
			//		<hs1_name>Central York High School</hs1_name>
			//		<hs2_name/>
			//		<grad_year>1999</grad_year>
			//		<hs1_id>21846</hs1_id>
			//		<hs2_id>0</hs2_id>
			//	</hs_info>
			if(mLocalName == "hs1_name")
			{
				mHighschoolInfo.HsName += content;
			}
			else if(mLocalName == "hs2_name")
			{
				mHighschoolInfo.Hs2Name += content;
			}
			else if(mLocalName == "grad_year")
			{
				mHighschoolInfo.GradYear += content;
			}
			else if(mLocalName == "hs1_id")
			{
				mHighschoolInfo.Hs1Id += content;
			}
			else if(mLocalName == "hs2_id")
			{
				mHighschoolInfo.Hs2Id += content;
			}
			break;
		}
		case eStatus:
		{
			//	<status>
			//		<message/>
			//		<time>0</time>
			//	</status>
			if(mLocalName == "message")
			{
				mStatus.Message += content;
			}
			else if(mLocalName == "time")
			{
				mStatus.Time += content;
			}
			break;
		}
		case eWorkHistory:
		{
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

			// no interesting information...
			break;
		}
		case eWorkHistory_WorkInfo:
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
			if(mLocalName == "company_name")
			{
				mWorkInfo.CompanyName += content;
			}
			else if(mLocalName == "position")
			{
				mWorkInfo.Position += content;
			}
			else if(mLocalName == "description")
			{
				mWorkInfo.Description += content;
			}
			else if(mLocalName == "start_date")
			{
				mWorkInfo.StartDate += content;
			}
			else if(mLocalName == "end_date")
			{
				mWorkInfo.EndDate += content;
			}
			break;
		}
		case eWorkHistory_WorkInfo_Location:
		{
			//		<location>
			//			<city>Palo Alto</city>
			//			<state>CA</state>
			//			<country>United States</country>
			//		</location>
			if(mLocalName == "city")
			{
				mWorkInfoLocation.City += content;
			}
			else if(mLocalName == "state")
			{
				mWorkInfoLocation.State += content;
			}
			else if(mLocalName == "country")
			{
				mWorkInfoLocation.City += content;
			}
			break;
		}
	}
}


void GetUsersInfo::ignorableWhitespace(const XMLChar ch[], int start, int length)
{
}


void GetUsersInfo::processingInstruction(const XMLString& target, const XMLString& data)
{
}


void GetUsersInfo::startPrefixMapping(const XMLString& prefix, const XMLString& uri)
{
}


void GetUsersInfo::endPrefixMapping(const XMLString& prefix)
{
}


void GetUsersInfo::skippedEntity(const XMLString& name)
{
}


void GetUsersInfo::handleResponse(istream & inputstream) const
{
	SAXParser parser;
	parser.setFeature(XMLReader::FEATURE_NAMESPACES, true);
	parser.setFeature(XMLReader::FEATURE_NAMESPACE_PREFIXES, true);
	parser.setContentHandler(const_cast<GetUsersInfo*>(this));
	
	try
	{
		SharedPtr<InputSource> inputSource(new InputSource(inputstream));
		parser.parse(inputSource);
		mSuccessCallback.call(mUserInfoList);
	}
	catch (Exception& e)
	{
		cerr << e.displayText() << endl;
		mErrorCallback.call(ErrorResponse(0,e.message(), KeyValue()));
	}
}
