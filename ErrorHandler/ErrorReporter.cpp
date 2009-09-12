#include "ErrorReporter.h"
#include <assert.h>


ErrorReporter * ErrorReporter::sInstance = 0;


Error::Error() :
	mErrorCode(0)
{
}


Error::Error(int inErrorCode) :
	mErrorCode(inErrorCode)
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


int Error::errorCode() const
{
	return mErrorCode;
}


const std::string & Error::errorMessage() const
{
	return mErrorMessage;
}

	
void Error::setErrorMessage(const std::string & inErrorMessage)
{
	mErrorMessage = inErrorMessage;
}


bool Error::isError() const
{
	return errorCode() != 0;
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
		return *mStack.top();
	}
	return mTopLevelError;
}


void ErrorReporter::reportError(const Error & inError)
{
	if (!mStack.empty())
	{
		mStack.top()->setErrorCode(inError.errorCode());
		mStack.top()->setErrorMessage(inError.errorMessage());
	}
	else
	{
		mTopLevelError = inError;
	}
}


void ErrorReporter::push(ScopedError * inError)
{
	mStack.push(inError);
}


void ErrorReporter::pop(ScopedError * inError)
{
	bool foundOnTop = mStack.top() == inError;
	assert(foundOnTop);
	if (foundOnTop)
	{
		mStack.pop();
	}
}


void ErrorReporter::propagate(ScopedError * inError)
{
	// Empty stack would mean that there are no ScopedErrorHandle objects in existence right now
	assert (!mStack.empty());

	// If only one element is on the stack, then we propagate to top-level-error.
	if (mStack.size() == 1)
	{
		mTopLevelError.setErrorCode(inError->errorCode());
	}
	// Otherwise we overwrite the parent error
	else
	{
		std::stack<ScopedError*>::container_type::const_iterator target = mStack._Get_container().end();
		--target;
		--target;
		ScopedError * parentErrorHandler = *target;
		*parentErrorHandler = *inError;
	}
}


ScopedError::ScopedError() :
	mPropagate(false)
{
	ErrorReporter::Instance().push(this);
}


ScopedError::~ScopedError()
{
	if (mPropagate)
	{
		ErrorReporter::Instance().propagate(this);
	}
	ErrorReporter::Instance().pop(this);
}


void ScopedError::propagate()
{
	mPropagate = true;
}


void ReportError(const Error & inError)
{
	ErrorReporter::Instance().reportError(inError);
}