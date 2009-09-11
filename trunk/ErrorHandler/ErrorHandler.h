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

	void dismiss();

private:
	void propagate();
	bool mDismissed;
};


class GlobalErrorHandler : public ErrorHandler
{
public:
	static void CreateInstance();
	
	static GlobalErrorHandler & Instance();
	
	static void DestroyInstance();

	int lastError() const;

	void postError(int inErrorCode);

private:
	friend class ScopedErrorHandler;

	void push(ScopedErrorHandler * inErrorHandler);

	void pop(ScopedErrorHandler * inErrorHandler);

	void propagate(ScopedErrorHandler * inErrorHandler);

	std::stack<ScopedErrorHandler*> mStack;
	static GlobalErrorHandler * sInstance;
};

#endif // ERRORHANDLER_H
