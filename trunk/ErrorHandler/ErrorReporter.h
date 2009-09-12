#ifndef ERRORREPORTER_H
#define ERRORREPORTER_H


#include <stack>


class Error
{
public:
	Error();

	Error(int inErrorCode);

	Error(int inErrorCode, const std::string & inErrorMessage);

	bool caughtError() const;

	int errorCode() const;

	void setErrorCode(int inErrorCode);

	const std::string & errorMessage() const;
	
	void setErrorMessage(const std::string & inErrorMessage);

private:
	int mErrorCode;
	std::string mErrorMessage;
};


class ErrorReporter;

class ErrorCatcher
{
public:
	ErrorCatcher();

	~ErrorCatcher();

	bool caughtError() const;

	const Error & error() const;

	void propagate();

private:
	friend class ErrorReporter;
	Error mError;
	bool mPropagate;
};


class ErrorReporter
{
public:
	static void CreateInstance();
	
	static ErrorReporter & Instance();
	
	static void DestroyInstance();

	const Error & lastError() const;

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
