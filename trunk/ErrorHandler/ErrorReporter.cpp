#include "ErrorReporter.h"
#include <assert.h>


namespace CppToys
{


	ErrorReporter * ErrorReporter::sInstance = 0;


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


	void Error::setErrorCode(int inErrorCode)
	{
		mErrorCode = inErrorCode;
	}


	int Error::code() const
	{
		return mErrorCode;
	}


	const std::string & Error::message() const
	{
		return mErrorMessage;
	}

		
	void Error::setErrorMessage(const std::string & inErrorMessage)
	{
		mErrorMessage = inErrorMessage;
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


	void ErrorReporter::CreateInstance()
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


	void ErrorReporter::DestroyInstance()
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


	void ErrorReporter::reportFailure(const Error & inError)
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
			--target;
			--target;
			ErrorCatcher * parentErrorHandler = *target;
			*parentErrorHandler = *inError;
		}
	}

	
	void Fail(int inErrorCode, const std::string & inErrorMessage)
	{
		ErrorReporter::Instance().reportFailure(Error(inErrorCode, inErrorMessage));
	}
	
	
	void Fail(const std::string & inErrorMessage)
	{
		ErrorReporter::Instance().reportFailure(Error(inErrorMessage));
	}


	void Fail(int inErrorCode)
	{
		ErrorReporter::Instance().reportFailure(Error(inErrorCode));
	}


} // namespace CppToys
