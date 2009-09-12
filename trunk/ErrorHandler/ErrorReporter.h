#ifndef ERRORREPORTER_H
#define ERRORREPORTER_H


#include <stack>


/**
 * Class Error bundles an error code with its message.
 * The default value for the error code is zero.
 * Error code zero means that no error has occured.
 * The error message can be empty.
 */
class Error
{
public:
	Error();

	Error(int inErrorCode);

	Error(int inErrorCode, const std::string & inErrorMessage);

	/**
	 * If no error has occurred then 0 is returned.
	 */
	int errorCode() const;

	void setErrorCode(int inErrorCode);

	const std::string & errorMessage() const;
	
	void setErrorMessage(const std::string & inErrorMessage);

private:
	int mErrorCode;
	std::string mErrorMessage;
};


/** Forward declaration */
class ErrorReporter;


/**
 * ErrorCatcher objects can be used to limit error handling to a certain scope.
 * A ErrorCatcher object should always be created on the stack, and usually at
 * the beginning of its surrounding scope.
 */
class ErrorCatcher
{
public:
	ErrorCatcher();

	~ErrorCatcher();

	/**
	 * Returns true if an error was reported during the lifetime of this object
	 * and within its scope (unless the error was caught by deeper nested
	 * ErrorCatcher object without being propagated).
	 */
	bool caughtError() const;

	/**
     * Returns the error object.
	 */
	const Error & error() const;

	/**
	 * Errors caught will be disposed of when this object's lifetime ends.
	 * If this is not desirable then you can use 'propagate' to forward the Error object to
	 * the nearest parent ErrorCatcher. If no ErrorCatcher is defined in a parent scope, then
	 * the error is set as the ErrorReporter's top level error.
	 */
	void propagate();

private:
	friend class ErrorReporter;
	Error mError;
	bool mPropagate;
};


/**
 * ErrorReporter is the class that you can use for reporting errors.
 */
class ErrorReporter
{
public:
	/**
	 * CreateInstance must be called once (and only once) at program startup, and
	 * before any ErrorCatcher objects are created.
	 */
	static void CreateInstance();
	

	/**
	 * Returns the singleton object.
	 */
	static ErrorReporter & Instance();
	
	/**
	 * DestroyInstance should be called before shutting down your program.
	 */
	static void DestroyInstance();

	/**
	 * Returns the last reported error. This means the error that is
	 * currently held by the deepest nested ErrorCatcher object.
	 * If no ErrorCatcher objects are currently defined the top level
	 * error object is returned.
	 */
	const Error & lastError() const;

	/**
	 * Use this method to report an error.
	 */
	void reportError(const Error & inError);

private:
	friend class ErrorCatcher;

	void push(ErrorCatcher * inError);

	void pop(ErrorCatcher * inError);

	void propagate(ErrorCatcher * inError);

	Error mTopLevelError;
	std::stack<ErrorCatcher*> mStack;
	static ErrorReporter * sInstance;
};

// Shortcut function for ErrorReporter::Instance().reportError(..).
void ReportError(const Error & inError);


#endif // ERRORREPORTER_H
