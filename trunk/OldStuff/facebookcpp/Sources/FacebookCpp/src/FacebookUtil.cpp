#include "FacebookCpp/FacebookUtil.h"
#include "FacebookCpp/Method/FacebookMethod.h"
#include "FacebookCpp/Method/FacebookSessionMethod.h"
#include "Poco/AutoPtr.h"
#include "Poco/MD5Engine.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Node.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/SAX/InputSource.h"
#include <iostream>


#ifdef WIN32
#include <shellapi.h>
#include <tchar.h>
#endif


using namespace Poco;
using namespace XML;
using namespace std;


namespace FacebookCpp
{


	bool isErrorResponse(const KeyValue & inKeyValue)
	{
		return inKeyValue.find("error_response") != inKeyValue.end(); // if contains "error_response" then error
	}

	map<string, string> parseSimpleXml(istream & inputstream)
	{
		map<string, string> result;
		InputSource src(inputstream);
		try
		{
			DOMParser parser;
			AutoPtr<Document> pDoc = parser.parse(&src);
			
			NodeIterator it(pDoc, NodeFilter::SHOW_ALL);
			Node* pNode = it.nextNode();
			while (pNode)
			{
				result[pNode->nodeName()] = pNode->innerText();
				pNode = it.nextNode();
			}
		}
		catch (Exception& exc)
		{
			cerr << exc.displayText() << endl;
		}
		return result;
	}


	vector<string> parseSimpleXmlElements(istream & inputstream, const string & inElementName, KeyValue & outKeyValue)
	{
		vector<string> results;
		InputSource src(inputstream);
		try
		{
			DOMParser parser;
			AutoPtr<Document> pDoc = parser.parse(&src);
			
			NodeIterator it(pDoc, NodeFilter::SHOW_ALL);
			Node* pNode = it.nextNode();
			while (pNode)
			{
				string nodeName = pNode->nodeName();
				string innerText = pNode->innerText();
				if(nodeName == inElementName)
				{
					results.push_back(innerText);
				}
				outKeyValue[nodeName] = innerText;
				pNode = it.nextNode();
			}
		}
		catch (Exception& exc)
		{
			cerr << exc.displayText() << endl;
		}
		return results;
	}

	wstring convertToUni16String( const string& inUTF8String)
	{
		#ifdef _UNICODE
		
			wstring result;
			size_t l = inUTF8String.length();
			
			for (size_t i = 0; i < l; i++)
			{
				char c = inUTF8String[i];
				
				if ((c & 0x80) == 0)
				{
					result += TCHAR(c & 0x7f);
				}
				else if ((c & 0xe0) == 0xc0)
				{
					if (i+1 >= l || (inUTF8String[i+1] & 0xc0) != 0x80)
					{
						return result;
					}
					
					result += (TCHAR)((c & 0x1f) << 6) | (inUTF8String[i+1] & 0x3f);
					
					i++;
				}
				else
				{
					if (i+2 >= l || (inUTF8String[i+1] & 0xc0) != 0x80 || (inUTF8String[i+2] & 0xc0) != 0x80)
					{
						return result;
					}
					
					result += (TCHAR) ((c & 0x0f) << 12) | ((inUTF8String[i+1] & 0x3f) << 6) | (inUTF8String[i+2] & 0x3f);
					
					i += 2;
				}
			}
			
			return result;
		
		#else
		
			return inUTF8String;
		
		#endif // _UNICODE
	}
	

	string generateSig(const FacebookMethod::Params & inParams, const string & inSecret)
	{
		stringstream buf;
		FacebookMethod::Params::const_iterator it = inParams.begin(), end = inParams.end();
		for(; it != end; ++it)
		{
			buf << it->first << "=" << it->second;
		}
		buf << inSecret;

		MD5Engine md5;
		md5.update(buf.str());

		return MD5Engine::digestToHex(md5.digest());

	}


	string generateParams( const FacebookMethod& inMethod, const string & api_key, const string & secret)
	{
		string result;

		FacebookMethod::Params params = inMethod.getParams();
		params["api_key"] = api_key;
		params["sig"] = generateSig(params, secret);
		return concatenateParams(params);
	}


	string generateParams( const FacebookSessionMethod& inMethod, const string & api_key)
	{
		string result;

		FacebookMethod::Params params = inMethod.getParams();
		params["api_key"] = api_key;
		params["call_id"] = inMethod.getParams()["call_id"];
		params["session_key"] = inMethod.getParams()["session_key"];
		params["sig"] = generateSig(params, inMethod.getParams()["secret"]);
		return concatenateParams(params);
	}

	string toCSV(const vector<string> & inStrings)
	{
		stringstream buf;
		vector<string>::const_iterator it = inStrings.begin(), end = inStrings.end();
		for(; it != end; ++it)
		{
			buf << *it << ",";
		}
		string commas = buf.str();
		if(commas.size() > 1)
		{
			commas = commas.substr(0, commas.size()-1);
		}
		return commas;
	}

	
	string concatenateParams(const FacebookMethod::Params & inParams)
	{
		stringstream buf;
		FacebookMethod::Params::const_iterator it = inParams.begin(), end = inParams.end();
		for(; it != end; ++it)
		{
			buf << it->first << "=" << it->second << "&";
		}
		string params = buf.str();
		if( params.size() > 0 )
		{
			params = params.substr(0, params.size()-1);
		}
		return params;
	}


	void launchBrowser(const string & inUrl)
	{	
	#ifdef WIN32
		ShellExecute(NULL,_T("open"),convertToUni16String(inUrl).c_str(),NULL, NULL, SW_SHOWNORMAL);
	#else
		system(("open " + inUrl).c_str());
	#endif
	}


} // namespace FacebookCpp