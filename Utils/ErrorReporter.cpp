#include "ErrorReporter.h"
#include <assert.h>


namespace Utils
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
        ErrorReporter::Instance().pop(this);
		if (mPropagate)
		{
            ErrorReporter::Instance().mStack.top()->attach(this);
		}
		
	}


	bool ErrorCatcher::hasCaught() const
	{
		return !mErrors.empty();
	}


	void ErrorCatcher::propagate()
	{
		mPropagate = true;
	}
    
    
    void ErrorCatcher::push(const Error & inError)
    {
        mErrors.push_back(inError);
    }
    
    
    void ErrorCatcher::attach(const ErrorCatcher * inErrorCatcher)
    {
        mChild.reset(new ErrorCatcher(*inErrorCatcher));
    }


	void ErrorReporter::Initialize()
	{
		assert(!sInstance);
		if (!sInstance)
		{
			sInstance = new ErrorReporter();

            // this cannot be done the constructor
            // because ErrorCatcher requires that
            // sInstance has been set.
            sInstance->mStack.push(new ErrorCatcher);
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


    ErrorReporter::ErrorReporter()
    {
    }
    
    
    ErrorReporter::~ErrorReporter()
    {
        assert(!mStack.empty());
        if (!mStack.empty())
        {
            delete mStack.top();
        }
    }


	//const Error & ErrorReporter::lastError() const
	//{
	//	if (!mStack.empty())
	//	{
	//		return mStack.top()->mError;
	//	}
	//	return mTopLevelErrorCatcher;
	//}


	void ErrorReporter::reportError(const Error & inError)
	{
        assert (!mStack.empty());
        if (!mStack.empty())
        {
            mStack.top()->push(inError);
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


	//void ErrorReporter::propagate(ErrorCatcher * inError)
	//{
	//	// Empty stack would mean that there are no ErrorCatcher objects in existence right now
	//	assert (!mStack.empty());

	//	// If only one element is on the stack, then we propagate to top-level-error.
	//	if (mStack.size() == 1)
	//	{
	//		mTopLevelErrorCatcher = inError->mError;
	//	}
	//	// Otherwise we overwrite the parent error
	//	else
	//	{
	//		std::stack<ErrorCatcher*>::container_type::const_iterator target = mStack._Get_container().end();
	//		--target; // make it point to the last object
	//		--target; // make it point to the one but last object
	//		**target = *inError;
	//	}
	//}

	
	void ReportError(int inErrorCode, const std::string & inErrorMessage)
	{
		ErrorReporter::Instance().reportError(Error(inErrorCode, inErrorMessage));
	}
	
	
	void ReportError(const std::string & inErrorMessage)
	{
		ErrorReporter::Instance().reportError(Error(inErrorMessage));
	}


	void ReportError(int inErrorCode)
	{
		ErrorReporter::Instance().reportError(Error(inErrorCode));
	}


} // namespace Utils
