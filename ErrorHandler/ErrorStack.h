#ifndef ERRORSTACK_H
#define ERRORSTACK_H


#include <stack>


namespace CppToys
{


	/**
	 * Class Error bundles an error code with its message.
	 * The error message can be empty.
	 */
	class Error
	{
	public:

		/**
		 * Default error codes.
		 */
		enum
		{
			SUCCEEDED = 0,
			FAILED = -1
		};

		// Error code defaults to SUCCEEDED.
		Error();

		Error(int inErrorCode);

		// Error code defaults to FAILED.
		Error(const std::string & inErrorMessage);

		Error(int inErrorCode, const std::string & inErrorMessage);

		int code() const;

		const std::string & message() const;

	private:
		int mErrorCode;
		std::string mErrorMessage;
	};


	/** Forward declaration */
	class ErrorStack;


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
		 * Caught Errors will be disposed of on destruction of the ErrorCatcher object.
		 * If this is not desirable then you can use 'rethrow' to forward the Error to
		 * the nearest parent ErrorCatcher. If no parent ErrorCatcher is defined, then
		 * the error is set as the ErrorStack's top level error.
		 * NOTE: No actual C++ exception is thrown here. So you still need to write a
		 *       return statement if you want to return to caller.
		 */
		void rethrow();

	private:
		friend class ErrorStack;
		Error mError;
		bool mPropagate;
	};


	/**
	 * ErrorStack serves as a global stack for ErrorCatcher objects.
	 */
	class ErrorStack
	{
	public:
		/**
		 * Initialize must be called once (and only once) at program startup, and
		 * before any ErrorCatcher objects are created.
		 */
		static void Initialize();
		

		/**
		 * Returns the singleton object.
		 */
		static ErrorStack & Instance();
		
		/**
		 * Finalize should be called before shutting down your program.
		 */
		static void Finalize();

		/**
		 * You can use this method to report an error, however
		 * the ThrowError functions below are more convenient.
		 * NOTE: Calling this will not cause an exception to be thrown, so
		 *       program flow will not be altered. If you want to return to
		 *       the caller you still have to write a return statement.
		 */
		void throwError(const Error & inError);

		/**
		 * Returns the last reported error. This means the error that is
		 * currently held by the most deeply nested ErrorCatcher object.
		 * If no ErrorCatcher objects are currently defined the top level
		 * error object is returned.
		 */
		const Error & lastError() const;

	private:
		friend class ErrorCatcher;

		void push(ErrorCatcher * inError);

		void pop(ErrorCatcher * inError);

		void rethrow(ErrorCatcher * inError);

		Error mTopLevelError;
		std::stack<ErrorCatcher*> mStack;
		static ErrorStack * sInstance;
	};


	/**
	 * ThrowError and its overloads are shorter versions 
	 * for ErrorStack::Instance().throwError(..) 
	 * NOTE: These functions don't throw an actual C++ exception. They only
	 *       notify the nearest ErrorCatcher that an error has occured. So
	 *       you still need to write the return statement (if returning
	 *       is desired, of course).
	 */
	void ThrowError(int inErrorCode, const std::string & inErrorMessage);


	/**
	 * ThrowError by only passing a message. Default error code will be used (FAILED).
	 */
	void ThrowError(const std::string & inErrorMessage);


	/**
	 * ThrowError by only passing an error code.
	 */
	void ThrowError(int inErrorCode);


} // namespace CppToys


#endif // ERRORSTACK_H
