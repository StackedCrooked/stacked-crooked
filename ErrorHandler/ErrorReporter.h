#ifndef ERRORREPORTER_H
#define ERRORREPORTER_H


#include <stack>


class Error
{
public:
	Error();

	Error(int inErrorCode);

	Error(int inErrorCode, const std::string & inErrorMessage);

	bool isError() const;

	int errorCode() const;

	void setErrorCode(int inErrorCode);

	const std::string & errorMessage() const;
	
	void setErrorMessage(const std::string & inErrorMessage);

private:
	int mErrorCode;
	std::string mErrorMessage;
};


class ScopedError : public Error
{
public:
	ScopedError();

	~ScopedError();

	void propagate();

private:
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
	friend class ScopedError;

	void push(ScopedError * inError);

	void pop(ScopedError * inError);

	void propagate(ScopedError * inError);

	Error mTopLevelError;
	std::stack<ScopedError*> mStack;
	static ErrorReporter * sInstance;
};

// Shortcut function for ErrorReporter::Instance().reportError(..).
void ReportError(const Error & inError);


#endif // ERRORREPORTER_H
