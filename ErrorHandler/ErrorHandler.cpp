#include "ErrorHandler.h"
#include <assert.h>


GlobalErrorHandler * GlobalErrorHandler::sInstance = 0;


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


void GlobalErrorHandler::CreateInstance()
{
	assert(!sInstance);
	if (!sInstance)
	{
		sInstance = new GlobalErrorHandler();
	}
}


GlobalErrorHandler & GlobalErrorHandler::Instance()
{
	assert(sInstance);
	return *sInstance;
}


void GlobalErrorHandler::DestroyInstance()
{
	assert(sInstance);
	if (sInstance)
	{
		delete sInstance;
		sInstance = 0;
	}
}


int GlobalErrorHandler::lastError() const
{
	if (!mStack.empty())
	{
		return mStack.top()->errorCode();
	}
	return 0;
}


void GlobalErrorHandler::postError(int inErrorCode)
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


void GlobalErrorHandler::push(ScopedErrorHandler * inErrorHandler)
{
	mStack.push(inErrorHandler);
}


void GlobalErrorHandler::pop(ScopedErrorHandler * inErrorHandler)
{
	bool foundOnTop = mStack.top() == inErrorHandler;
	assert(foundOnTop);
	if (foundOnTop)
	{
		mStack.pop();
	}
}


void GlobalErrorHandler::propagate(ScopedErrorHandler * inErrorHandler)
{
	// Empty stack would mean that there are no ScopedErrorHandle objects in existence right now
	assert (!mStack.empty());

	// We must propagate the GlobalErrorHandler
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
	mDismissed(false)
{
	GlobalErrorHandler::Instance().push(this);
}


ScopedErrorHandler::~ScopedErrorHandler()
{
	if (!mDismissed)
	{
		GlobalErrorHandler::Instance().propagate(this);
	}
	GlobalErrorHandler::Instance().pop(this);
}


void ScopedErrorHandler::dismiss()
{
	mDismissed = true;
}