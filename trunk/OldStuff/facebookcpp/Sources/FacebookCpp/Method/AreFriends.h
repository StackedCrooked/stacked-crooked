#ifndef __FacebookCpp_AreFriends_h__
#define __FacebookCpp_AreFriends_h__


#include "FacebookCpp/Method/FacebookSessionMethod.h"
#include "FacebookCpp/Entity/FriendInfo.h"
#include "Poco/SAX/ContentHandler.h"


namespace FacebookCpp
{


class AreFriends :	public FacebookSessionMethod,
										public Poco::XML::ContentHandler
{
public:

	typedef std::vector<FriendInfo> ResponseType;

	AreFriends
	(
		const Session & inSession,
		const std::vector<std::string> & inUids1,
		const std::vector<std::string> & inUids2,
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
	FacebookCpp::FbSharedCallback<ResponseType> mSuccessCallback;
	std::vector<std::string> mUids1;
	std::vector<std::string> mUids2;

	// Helper variables during parsing
	enum FriendInfoElement
	{
		eUid1,
		eUid2,
		eAreFriends,
		eOther
	};
	FriendInfoElement mCurrentElement;
	Poco::SharedPtr<std::stringstream> mCurrentUid1;
	Poco::SharedPtr<std::stringstream> mCurrentUid2;
	Poco::SharedPtr<std::stringstream> mCurrentAreFriends;
	std::vector<FriendInfo> mFriendInfoList;
};


} // namespace FacebookCpp


#endif // __FacebookCpp_AreFriends_h__