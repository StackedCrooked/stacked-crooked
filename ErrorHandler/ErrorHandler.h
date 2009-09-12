#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H


#include <stack>


class Error
{
public:
	Error();

	bool isError() const;

	int errorCode() const;

	void setErrorCode(int inErrorCode);

	const std::string & errorMessage();
	
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

	int lastError() const;

	void reportError(int inErrorCode);

private:
	friend class ScopedError;

	void push(ScopedError * inError);

	void pop(ScopedError * inError);

	void propagate(ScopedError * inError);

	Error mTopLevelError;
	std::stack<ScopedError*> mStack;
	static ErrorReporter * sInstance;
};


#endif // ERRORHANDLER_H
