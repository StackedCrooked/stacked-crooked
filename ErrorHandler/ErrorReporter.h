#ifndef ERRORREPORTER_H
#define ERRORREPORTER_H


#include <stack>


namespace CppToys
{


	/**
	 * Class Error bundles an error code with its message.
	 * The default value for the error code is zero.
	 * Error code zero means that no error has occured.
	 * The error message can be empty.
	 */
	class Error
	{
	public:
		// Error code defaults to 0 (no error).
		Error();

		Error(int inErrorCode);

		// Error code defaults to 1.
		Error(const std::string & inErrorMessage);

		Error(int inErrorCode, const std::string & inErrorMessage);

		/**
		 * If no error has occurred then 0 is returned.
		 */
		int code() const;

		void setErrorCode(int inErrorCode);

		const std::string & message() const;
		
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
		 * and not caught by a deeper nested ErrorCatcher object.
		 */
		bool hasCaught() const;

		/**
		 * Returns the error message
		 */
		const std::string & message() const
		{ return mError.message(); }

		/**
		 * Returns the error code. Zero means no error.
		 */
		int code() const
		{ return mError.code(); }

		/**
		 * Errors caught will be disposed of when this object's lifetime ends.
		 * If this is not desirable then you can use 'rethrow' to forward the Error object to
		 * the nearest parent ErrorCatcher. If no ErrorCatcher is defined in a parent scope, then
		 * the error is set as the ErrorReporter's top level error.
		 * NOTE: Unlike c++ exceptions, calling this method will not alter program flow in any way. 
		 *       So you still need to write a return statement.
		 */
		void rethrow();

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
		 * NOTE: Calling this will not cause an exception to be thrown, so
		 *       program flow will not be altered. If you want to return to
		 *       the caller you still have to write a return statement.
		 */
		void reportFailure(const Error & inError);

	private:
		friend class ErrorCatcher;

		void push(ErrorCatcher * inError);

		void pop(ErrorCatcher * inError);

		void rethrow(ErrorCatcher * inError);

		Error mTopLevelError;
		std::stack<ErrorCatcher*> mStack;
		static ErrorReporter * sInstance;
	};

	void Fail(int inErrorCode, const std::string & inErrorMessage);

	void Fail(const std::string & inErrorMessage);

	void Fail(int inErrorCode);


} // namespace CppToys


#endif // ERRORREPORTER_H
