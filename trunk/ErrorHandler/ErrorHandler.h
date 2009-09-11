#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H


#include <stack>


class ErrorHandler
{
public:
	ErrorHandler();

	bool hasError() const;

	int errorCode() const;

	void setErrorCode(int inErrorCode);

private:
	int mErrorCode;
};


class ScopedErrorHandler : public ErrorHandler
{
public:
	ScopedErrorHandler();

	~ScopedErrorHandler();

	void propagate();

private:
	bool mPropagate;
};


class ErrorReporter : public ErrorHandler
{
public:
	static void CreateInstance();
	
	static ErrorReporter & Instance();
	
	static void DestroyInstance();

	int lastError() const;

	void postError(int inErrorCode);

private:
	friend class ScopedErrorHandler;

	void push(ScopedErrorHandler * inErrorHandler);

	void pop(ScopedErrorHandler * inErrorHandler);

	void propagate(ScopedErrorHandler * inErrorHandler);

	std::stack<ScopedErrorHandler*> mStack;
	static ErrorReporter * sInstance;
};


void ReportError(int inErrorCode);


#endif // ERRORHANDLER_H
