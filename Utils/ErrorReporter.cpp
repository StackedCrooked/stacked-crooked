#include "ErrorReporter.h"
#include <sstream>
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
		mPropagate(false),
        mOwns(true), // the original object, created on the stack, must do the cleanup
        mDisableLogging(false)
	{
		ErrorReporter::Instance().push(this);
	}


	ErrorCatcher::~ErrorCatcher()
	{
        if (mOwns)
        {
            ErrorReporter::Instance().pop(this);
		    if (mPropagate)
		    {
                ErrorReporter::Instance().mStack.top()->setChild(this);
            }
            else 
            {
                if (!mDisableLogging)
                {
                    log();
                }
            }
        }
	}

    
    ErrorCatcher::ErrorCatcher(const ErrorCatcher & rhs) :
        mOwns(false), // copy is not responsible for cleanup
        mErrors(rhs.mErrors),
        mChild(rhs.mChild),
		mPropagate(rhs.mPropagate)
    {
    }
        
        
    void ErrorCatcher::disableLogging(bool inDisableLogging)
    {
        mDisableLogging = inDisableLogging;
    }


	bool ErrorCatcher::hasCaught() const
	{
		return mChild || !mErrors.empty();
	}


    void ErrorCatcher::log()
    {
        ErrorReporter::Instance().log(this);
    }


	void ErrorCatcher::propagate()
	{
		mPropagate = true;
	}
    
    
    void ErrorCatcher::push(const Error & inError)
    {
        mErrors.push_back(inError);
    }
    
    
    void ErrorCatcher::setChild(const ErrorCatcher * inErrorCatcher)
    {
        mChild.reset(new ErrorCatcher(*inErrorCatcher));
    }


	void ErrorReporter::Initialize()
	{
		assert(!sInstance);
		if (!sInstance)
		{
			sInstance = new ErrorReporter();

            // this cannot be done in the constructor
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


    void ErrorReporter::setLogger(const LogFunction & inLogFunction)
    {
        mLogFunction = inLogFunction;
    }


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
    
    
    void ErrorReporter::log(ErrorCatcher * inErrorCatcher)
    {
        if (!mLogFunction)
        {
            return;
        }

        struct Helper
        {
            static void GetErrorMessage(const ErrorCatcher & inErrorCatcher, std::stringstream & ss)
            {
                for (size_t idx = 0; idx != inErrorCatcher.errors().size(); ++idx)
                {
                    const Error & error = inErrorCatcher.errors()[idx];
                    if (idx > 0)
                    {
                        ss << "Therefore: ";
                    }
                    ss << error.message();
                    
                    // show the error code, unless if it is the default
                    if (error.code() != Error::FAILED)
                    {
                        ss << " (Code: " << error.code() << ")";
                    }
                    ss << "\n";
                }
                if (inErrorCatcher.child())
                {
                    GetErrorMessage(*inErrorCatcher.child(), ss);
                }
            }
        };
        std::stringstream ss;
        Helper::GetErrorMessage(*inErrorCatcher, ss);

        std::string message(ss.str());
        if (!message.empty())
        {
            mLogFunction(ss.str());
        }
    }

	
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
