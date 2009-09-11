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

	bool isError() const;

	int errorCode() const;

	void setErrorCode(int inErrorCode);

	void propagate();

private:
	ErrorInfo mErrorInfo;
	bool mPropagate;
};


class ErrorReporter
{
public:
	static void CreateInstance();
	
	static ErrorReporter & Instance();
	
	static void DestroyInstance();

	int lastError() const;

	void reportError(int inErrorCode);

private:
	friend class ScopedError;

	void push(ScopedError * inError);

	void pop(ScopedError * inError);

	void propagate(ScopedError * inError);

	ErrorInfo mTopLevelError;
	std::stack<ScopedError*> mStack;
	static ErrorReporter * sInstance;
};


#endif // ERRORHANDLER_H
