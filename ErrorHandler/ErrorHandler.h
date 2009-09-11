#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H


#include <stack>


class ErrorInfo
{
public:
	ErrorInfo();

	int errorCode() const;

	void setErrorCode(int inErrorCode);

private:
	int mErrorCode;
};


class ScopedError
{
public:
	ScopedError();

	~ScopedError();

	bool hasError() const;

	int errorCode() const;

	void setErrorCode(int inErrorCode);

	void propagate();

private:
	ErrorInfo mErrorInfo;
	bool mPropagate;
};


class ErrorReporter : public ErrorInfo
{
public:
	static void CreateInstance();
	
	static ErrorReporter & Instance();
	
	static void DestroyInstance();

	int lastError() const;

	void postError(int inErrorCode);

private:
	friend class ScopedError;

	void push(ScopedError * inErrorHandler);

	void pop(ScopedError * inErrorHandler);

	void propagate(ScopedError * inErrorHandler);

	std::stack<ScopedError*> mStack;
	static ErrorReporter * sInstance;
};


void ReportError(int inErrorCode);


#endif // ERRORHANDLER_H
