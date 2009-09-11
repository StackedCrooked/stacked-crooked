#include "ErrorHandler.h"
#include <assert.h>


ErrorReporter * ErrorReporter::sInstance = 0;


ErrorHandler::ErrorHandler() :
	mErrorCode(0)
{
}


bool ErrorHandler::hasError() const
{
	return mErrorCode != 0;
}


void ErrorHandler::setErrorCode(int inErrorCode)
{
	mErrorCode = inErrorCode;
}


int ErrorHandler::errorCode() const
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


void ErrorReporter::postError(int inErrorCode)
{
	if (!mStack.empty())
	{
		mStack.top()->setErrorCode(inErrorCode);
	}
	else
	{
		setErrorCode(inErrorCode);
	}
}


void ErrorReporter::push(ScopedErrorHandler * inErrorHandler)
{
	mStack.push(inErrorHandler);
}


void ErrorReporter::pop(ScopedErrorHandler * inErrorHandler)
{
	bool foundOnTop = mStack.top() == inErrorHandler;
	assert(foundOnTop);
	if (foundOnTop)
	{
		mStack.pop();
	}
}


void ErrorReporter::propagate(ScopedErrorHandler * inErrorHandler)
{
	// Empty stack would mean that there are no ScopedErrorHandle objects in existence right now
	assert (!mStack.empty());

	// We must propagate the ErrorReporter
	if (mStack.size() == 1)
	{
		setErrorCode(inErrorHandler->errorCode());
	}
	else
	{
		std::stack<ScopedErrorHandler*>::container_type::const_iterator target = mStack._Get_container().end();
		--target;
		--target;
		ScopedErrorHandler * parentErrorHandler = *target;
		parentErrorHandler->setErrorCode(inErrorHandler->errorCode());
	}
}


ScopedErrorHandler::ScopedErrorHandler() :
	mPropagate(false)
{
	ErrorReporter::Instance().push(this);
}


ScopedErrorHandler::~ScopedErrorHandler()
{
	if (mPropagate)
	{
		ErrorReporter::Instance().propagate(this);
	}
	ErrorReporter::Instance().pop(this);
}


void ScopedErrorHandler::propagate()
{
	mPropagate = true;
}


void ReportError(int inErrorCode)
{
	ErrorReporter::Instance().postError(inErrorCode);
}