#include "ErrorStack.h"
#include <assert.h>


namespace CppToys
{


	ErrorStack * ErrorStack::sInstance = 0;


	Error::Error() :
		mErrorCode(0)
	{
	}


	Error::Error(int inErrorCode) :
		mErrorCode(inErrorCode)
	{
	}
		
		
	Error::Error(const std::string & inErrorMessage) :
		mErrorCode(1),
		mErrorMessage(inErrorMessage)
	{
	}


	Error::Error(int inErrorCode, const std::string & inErrorMessage) :
		mErrorCode(inErrorCode),
		mErrorMessage(inErrorMessage)
	{
	}


	int Error::code() const
	{
		return mErrorCode;
	}


	const std::string & Error::message() const
	{
		return mErrorMessage;
	}


	ErrorCatcher::ErrorCatcher() :
		mPropagate(false)
	{
		ErrorStack::Instance().push(this);
	}


	ErrorCatcher::~ErrorCatcher()
	{
		if (mPropagate)
		{
			ErrorStack::Instance().rethrow(this);
		}
		ErrorStack::Instance().pop(this);
	}


	bool ErrorCatcher::hasCaught() const
	{
		return mError.code() != 0;
	}


	void ErrorCatcher::rethrow()
	{
		mPropagate = true;
	}


	void ErrorStack::Initialize()
	{
		assert(!sInstance);
		if (!sInstance)
		{
			sInstance = new ErrorStack();
		}
	}


	ErrorStack & ErrorStack::Instance()
	{
		assert(sInstance);
		return *sInstance;
	}


	void ErrorStack::Finalize()
	{
		assert(sInstance);
		if (sInstance)
		{
			delete sInstance;
			sInstance = 0;
		}
	}


	const Error & ErrorStack::lastError() const
	{
		if (!mStack.empty())
		{
			return mStack.top()->mError;
		}
		return mTopLevelError;
	}


	void ErrorStack::throwError(const Error & inError)
	{
		if (!mStack.empty())
		{
			mStack.top()->mError = inError;
		}
		else
		{
			mTopLevelError = inError;
		}
	}


	void ErrorStack::push(ErrorCatcher * inError)
	{
		mStack.push(inError);
	}


	void ErrorStack::pop(ErrorCatcher * inError)
	{
		bool foundOnTop = mStack.top() == inError;
		assert(foundOnTop);
		if (foundOnTop)
		{
			mStack.pop();
		}
	}


	void ErrorStack::rethrow(ErrorCatcher * inError)
	{
		// Empty stack would mean that there are no ErrorCatcher objects in existence right now
		assert (!mStack.empty());

		// If only one element is on the stack, then we rethrow to top-level-error.
		if (mStack.size() == 1)
		{
			mTopLevelError = inError->mError;
		}
		// Otherwise we overwrite the parent error
		else
		{
			std::stack<ErrorCatcher*>::container_type::const_iterator target = mStack._Get_container().end();
			--target; // make it point to the last object
			--target; // make it point to the one but last object
			**target = *inError;
		}
	}

	
	void ThrowError(int inErrorCode, const std::string & inErrorMessage)
	{
		ErrorStack::Instance().throwError(Error(inErrorCode, inErrorMessage));
	}
	
	
	void ThrowError(const std::string & inErrorMessage)
	{
		ErrorStack::Instance().throwError(Error(inErrorMessage));
	}


	void ThrowError(int inErrorCode)
	{
		ErrorStack::Instance().throwError(Error(inErrorCode));
	}


} // namespace CppToys
