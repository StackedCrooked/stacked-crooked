#ifndef __FacebookCpp_FacebookUtil_h__
#define __FacebookCpp_FacebookUtil_h__


#include "FacebookCpp/Defines.h"
#include "Poco/Delegate.h"
#include "Poco/SharedPtr.h"
#include <map>
#include <sstream>
#include <string>
#include <vector>


namespace FacebookCpp
{

	class FacebookMethod;

	class FacebookSessionMethod;
	
	bool isErrorResponse(const KeyValue & inKeyValue);

	std::map<std::string, std::string> parseSimpleXml(std::istream & inputstream);

	std::vector<std::string> parseSimpleXmlElements(std::istream & inputstream, const std::string & inElementName, KeyValue & outKeyValue);

	std::wstring convertToUni16String(const std::string& inString);

	void launchBrowser(const std::string & inUrl);

	std::string toCSV(const std::vector<std::string> & inStrings);

	// Returns all combined pairs from a list of items.
	// For example if you have five people meeting, then return a list of all handshakes given.
	template<class T>
	std::vector<std::pair<T,T> > getPairCombinations(const std::vector<T> & inItems);

	template<class T>
	void splitPairs(const std::vector<std::pair<T,T> > & inPairs, std::vector<T> & outFirst, std::vector<T> & outSecond);

	std::string concatenateParams(const std::map<std::string, std::string> & inParams);

	std::string generateSig(const std::map<std::string, std::string> & inParams, const std::string & inSecret);

	std::string generateParams( const FacebookMethod& inMethod, const std::string & api_key, const std::string & secret);

	std::string generateParams( const FacebookSessionMethod & inMethod, const std::string & api_key);
	
	// Reference holds a reference to an object.
	// This can be stored as a reference itself, thereby enables ref to ref, which is illigal in C++.
	template<class T>
	class Reference
	{
		T& mRef;
	public:
		Reference(T& inT) : mRef(inT) {}
		T& get() { return mRef; }
	};



	// Facebook delegate
	template <class TObj, class TArgs> 
	class FbCallback: public Poco::AbstractDelegate<TArgs>
	{
	public:
		typedef void (TObj::*NotifyMethod)(const TArgs &);

		FbCallback(TObj* obj, NotifyMethod method):
			AbstractDelegate<TArgs>(obj),
			_receiverObject(obj), 
			_receiverMethod(method)
		{
		}

		FbCallback(const FbCallback& delegate):
			AbstractDelegate<TArgs>(delegate),
			_receiverObject(delegate._receiverObject),
			_receiverMethod(delegate._receiverMethod)
		{
		}

		~FbCallback()
		{
		}
		
		FbCallback& operator = (const FbCallback& delegate)
		{
			if (&delegate != this)
			{
				this->_pTarget        = delegate._pTarget;
				this->_receiverObject = delegate._receiverObject;
				this->_receiverMethod = delegate._receiverMethod;
			}
			return *this;
		}

		bool notify(const void* sender, TArgs& arguments)
		{
			(_receiverObject->*_receiverMethod)(arguments);
			return true; // a "standard" delegate never expires
		}

		AbstractDelegate<TArgs>* clone() const
		{
			return new FbCallback(*this);
		}

	protected:
		TObj*        _receiverObject;
		NotifyMethod _receiverMethod;

	private:
		FbCallback();
	};
	

	template<class ResponseType>
	class FbSharedCallback
	{
	public:
		FbSharedCallback(const Poco::AbstractDelegate<ResponseType> & inDelegate):
			mDelegate(inDelegate.clone())
		{
		}
		~FbSharedCallback()
		{
		}
		void call(const ResponseType& arguments) const
		{
			mDelegate->notify(0, const_cast<ResponseType&>(arguments));
		}
	private:
		mutable Poco::SharedPtr< Poco::AbstractDelegate<ResponseType> > mDelegate;
	};

} // namespace FacebookCpp



// Implementations of template functions


template<class T>
std::vector<std::pair<T,T> > FacebookCpp::getPairCombinations(const std::vector<T> & inItems)
{
	std::vector<std::pair<T,T> > result;
	if(inItems.size() <= 1)
	{
		return result;
	}
	else if(inItems.size() == 2)
	{
		result.push_back(std::pair<T,T>(inItems[0],inItems[1]));
		return result;
	}
	else
	{
		T head = inItems[0];
		std::vector<T> tail;
		for(size_t idx = 1; idx < inItems.size(); ++idx)
		{
			T item = inItems[idx];
			result.push_back(std::pair<T,T>(head, item));
			tail.push_back(item);
		}
		std::vector<std::pair<T,T> > tailCombinations = getPairCombinations(tail);
		std::copy(tailCombinations.begin(), tailCombinations.end(), std::back_inserter(result)); 
		return result;
	}
}


template<class T>
void FacebookCpp::splitPairs(const std::vector<std::pair<T,T> > & inPairs, std::vector<T> & outFirst, std::vector<T> & outSecond)
{
	std::vector<std::pair<T,T> >::const_iterator it = inPairs.begin(), end = inPairs.end();
	for(; it != end; ++it)
	{
		outFirst.push_back(it->first);
		outSecond.push_back(it->second);
	}
}


#endif // __FacebookCpp_FacebookUtil_h__