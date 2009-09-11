#include "ErrorHandler.h"
#include <assert.h>


ErrorReporter * ErrorReporter::sInstance = 0;


ErrorInfo::ErrorInfo() :
	mErrorCode(0)
{
}


void ErrorInfo::setErrorCode(int inErrorCode)
{
	mErrorCode = inErrorCode;
}


int ErrorInfo::errorCode() const
{
	return mErrorCode;
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


int ErrorReporter::lastError() const
{
	if (!mStack.empty())
	{
		return mStack.top()->errorCode();
	}
	return 0;
}


void ErrorReporter::reportError(int inErrorCode)
{
	if (!mStack.empty())
	{
		mStack.top()->setErrorCode(inErrorCode);
	}
	else
	{
		mTopLevelError.setErrorCode(inErrorCode);
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


bool ScopedError::isError() const
{
	return mErrorInfo.errorCode() != 0;
}


int ScopedError::errorCode() const
{
	return mErrorInfo.errorCode();
}


void ScopedError::setErrorCode(int inErrorCode)
{
	mErrorInfo.setErrorCode(inErrorCode);
}


void ScopedError::propagate()
{
	mPropagate = true;
}


void ReportError(int inErrorCode)
{
	ErrorReporter::Instance().reportError(inErrorCode);
}