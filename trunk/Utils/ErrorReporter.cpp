#include "ErrorStack.h"
#include <assert.h>


namespace CppToys
{


	ErrorReporter * ErrorReporter::sInstance = 0;


	Error::Error() :
		mErrorCode(Error::SUCCEEDED)
	{
	}


	Error::Error(int inErrorCode) :
		mErrorCode(inErrorCode)
	{
	}
		
		
	Error::Error(const std::string & inErrorMessage) :
		mErrorCode(Error::FAILED),
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
		ErrorReporter::Instance().push(this);
	}


	ErrorCatcher::~ErrorCatcher()
	{
		if (mPropagate)
		{
			ErrorReporter::Instance().rethrow(this);
		}
		ErrorReporter::Instance().pop(this);
	}


	bool ErrorCatcher::hasCaught() const
	{
		return mError.code() != 0;
	}


	void ErrorCatcher::rethrow()
	{
		mPropagate = true;
	}


	void ErrorReporter::Initialize()
	{
		assert(!sInstance);
		if (!sInstance)
		{
			sInstance = new ErrorReporter();
		}
	}


	ErrorReporter & ErrorReporter::Instance()
	{
		assert(sInstance);
		return *sInstance;
	}


	void ErrorReporter::Finalize()
	{
		assert(sInstance);
		if (sInstance)
		{
			delete sInstance;
			sInstance = 0;
		}
	}


	const Error & ErrorReporter::lastError() const
	{
		if (!mStack.empty())
		{
			return mStack.top()->mError;
		}
		return mTopLevelError;
	}


	void ErrorReporter::reportError(const Error & inError)
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


	void ErrorReporter::push(ErrorCatcher * inError)
	{
		mStack.push(inError);
	}


	void ErrorReporter::pop(ErrorCatcher * inError)
	{
		bool foundOnTop = mStack.top() == inError;
		assert(foundOnTop);
		if (foundOnTop)
		{
			mStack.pop();
		}
	}


	void ErrorReporter::rethrow(ErrorCatcher * inError)
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
		ErrorReporter::Instance().reportError(Error(inErrorCode, inErrorMessage));
	}
	
	
	void ThrowError(const std::string & inErrorMessage)
	{
		ErrorReporter::Instance().reportError(Error(inErrorMessage));
	}


	void ThrowError(int inErrorCode)
	{
		ErrorReporter::Instance().reportError(Error(inErrorCode));
	}


} // namespace CppToys
