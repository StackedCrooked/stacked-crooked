#ifndef __FacebookCpp_GetUsersInfo_h__
#define __FacebookCpp_GetUsersInfo_h__


#include "FacebookCpp/Entity/UserInfo.h"
#include "FacebookCpp/Method/FacebookSessionMethod.h"
#include "Poco/SAX/ContentHandler.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>


namespace FacebookCpp
{


class GetUsersInfo :	public FacebookSessionMethod,
											public Poco::XML::ContentHandler
{
public:
	typedef std::vector<FacebookCpp::UserInfo> ResponseType;

	GetUsersInfo
	(
		const Session & inSession,
		const std::vector<std::string> & inUids,
		const Poco::AbstractDelegate<ResponseType> & inSuccessCallback,
		const Poco::AbstractDelegate<ErrorResponse> & inErrorCallback
	);

	virtual void handleResponse(std::istream & inputstream) const;
	
	// Poco::XML::ContentHander methods
	virtual void setDocumentLocator(const Poco::XML::Locator* loc);
	virtual void startDocument();
	virtual void endDocument();
	virtual void startElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname, const Poco::XML::Attributes& attrList);
	virtual void endElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname);
	virtual void characters(const Poco::XML::XMLChar ch[], int start, int length);
	virtual void ignorableWhitespace(const Poco::XML::XMLChar ch[], int start, int length);
	virtual void processingInstruction(const Poco::XML::XMLString& target, const Poco::XML::XMLString& data);
	virtual void startPrefixMapping(const Poco::XML::XMLString& prefix, const Poco::XML::XMLString& uri);
	virtual void endPrefixMapping(const Poco::XML::XMLString& prefix);
	virtual void skippedEntity(const Poco::XML::XMLString& name);

private:
	std::vector<std::string> mUids;
	FacebookCpp::FbSharedCallback<ResponseType> mSuccessCallback;
	FacebookCpp::UserInfo mUserInfo;
	std::vector<UserInfo> mUserInfoList;
	
	
	enum FbStruct
	{
		eNone,
		eAffiliationList,
		eAffiliationList_Affiliation,
		eHomeTownLocation,
		eCurrentLocation,
		eEducationHistory,
		eEducationHistory_EducationInfo,
		eEducationHistory_EducationInfo_Concentrations,
		eHighschoolInfo,
		eMeetingFor,
		eMeetingSex,
		eStatus,
		eWorkHistory,
		eWorkHistory_WorkInfo,
		eWorkHistory_WorkInfo_Location
	};
	static std::map<std::string, FbStruct> sMapping_ElementName_FbStruct;
	FbStruct mCurrentStruct;
	std::string mLocalName;

	std::vector<UserInfo::Affiliation> mAffiliations;
	UserInfo::Affiliation mAffiliation;
	UserInfo::HomeTownLocation mHomeTownLocation;	
	UserInfo::CurrentLocation mCurrentLocation;	
	
	std::vector<UserInfo::EducationInfo> mEducationHistory;
	UserInfo::EducationInfo mEducationInfo;
	UserInfo::Location mEducationInfoLocation;
	std::vector<std::string> mEducationConcentrations;
	std::string mEducationConcentration;

	UserInfo::HighschoolInfo mHighschoolInfo;
	UserInfo::Status mStatus;

	std::vector<UserInfo::WorkInfo> mWorkHistory;
	UserInfo::WorkInfo mWorkInfo;
	UserInfo::Location mWorkInfoLocation;

	std::string mCurrentString;
};


}; // namespace FacebookCpp


#endif //  __FacebookCpp_GetUsersInfo_h__
