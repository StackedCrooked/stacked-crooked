#include "FacebookCpp/Method/AreFriends.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/SAX/SAXParser.h"
#include "Poco/String.h"
#include <iostream>


using namespace FacebookCpp;
using namespace Poco;
using namespace XML;
using namespace std;


AreFriends::AreFriends
(
	const Session & inSession,
	const vector<string> & inUids1,
	const vector<string> & inUids2,
	const AbstractDelegate<ResponseType> & inSuccessCallback,
	const AbstractDelegate<ErrorResponse> & inErrorCallback
):
	FacebookSessionMethod(inSession, "Friends.areFriends", inErrorCallback),
	mUids1(inUids1),
	mUids2(inUids2),
	mSuccessCallback(inSuccessCallback),
	mCurrentElement(eOther)
{
	mParams["uids1"] = toCSV(inUids1);
	mParams["uids2"] = toCSV(inUids2);
}


void AreFriends::setDocumentLocator(const Locator* loc)
{
}


void AreFriends::startDocument()
{
}


void AreFriends::endDocument()
{
}


void AreFriends::startElement(const XMLString& uri, const XMLString& localName, const XMLString& qname, const Attributes& attrList)
{
	if(localName == "uid1")
	{
		mCurrentElement = eUid1;
		mCurrentUid1 = new stringstream;
	}
	else if(localName == "uid2")
	{
		mCurrentElement = eUid2;
		mCurrentUid2 = new stringstream;
	}
	else if(localName == "are_friends")
	{
		mCurrentElement = eAreFriends;
		mCurrentAreFriends = new stringstream;
	}
	else
	{
		mCurrentElement = eOther;
	}
}


void AreFriends::endElement(const XMLString& uri, const XMLString& localName, const XMLString& qname)
{
	if(localName == "friend_info")
	{
		mFriendInfoList.push_back
		(
			FriendInfo
			(
				trim(mCurrentUid1->str()),
				trim(mCurrentUid2->str()),
				trim(mCurrentAreFriends->str()) == "1"
			)
		);
		// Clear the stringstream
		//mCurrentUid1.str("");
		//mCurrentUid2.str("");
		//mCurrentAreFriends.str("");
		mCurrentElement = eOther;
	}
}


void AreFriends::characters(const XMLChar ch[], int start, int length)
{
	switch(mCurrentElement)
	{
		case eUid1:
		{
			*mCurrentUid1 << string(ch + start, length);
			break;
		}
		case eUid2:
		{
			*mCurrentUid2 << string(ch + start, length);
			break;
		}
		case eAreFriends:
		{
			*mCurrentAreFriends << string(ch + start, length);
			break;
		}
		default:
		{
			break;
		}
	}
}


void AreFriends::ignorableWhitespace(const XMLChar ch[], int start, int length)
{
}


void AreFriends::processingInstruction(const XMLString& target, const XMLString& data)
{
}


void AreFriends::startPrefixMapping(const XMLString& prefix, const XMLString& uri)
{
}


void AreFriends::endPrefixMapping(const XMLString& prefix)
{
}


void AreFriends::skippedEntity(const XMLString& name)
{
}


void AreFriends::handleResponse(istream & inputstream) const
{
	SAXParser parser;
	parser.setFeature(XMLReader::FEATURE_NAMESPACES, true);
	parser.setFeature(XMLReader::FEATURE_NAMESPACE_PREFIXES, true);
	parser.setContentHandler(const_cast<AreFriends*>(this));
	
	try
	{
		SharedPtr<InputSource> inputSource(new InputSource(inputstream));
		parser.parse(inputSource);
		mSuccessCallback.call(mFriendInfoList);
	}
	catch (Exception& e)
	{
		cerr << e.displayText() << endl;
		mErrorCallback.call(ErrorResponse(0,e.message(), KeyValue()));
	}
}