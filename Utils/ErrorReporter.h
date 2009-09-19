#ifndef ERRORSTACK_H
#define ERRORSTACK_H


#include <stack>
#include <vector>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>


namespace Utils
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

        ErrorCatcher(const ErrorCatcher & inErrorCatcher);

		~ErrorCatcher();

        /**
         * Logging is enabled by default and will occur on destruction
         * of the ErrorCatcher object. This call allows you to disable
         * this behavior.
         * This is handy for creating a 'error silencer' object.
         */
        void disableLogging(bool inDisable);
        

		/**
		 * Returns true if an error was reported during the lifetime of this object
		 * and not caught by a deeper nested ErrorCatcher object.
		 */
		bool hasCaught() const;

		/**
		 * On destruction the ErrorCatcher will log its error information.
         * However, you can also propagate the error, so that the next
         * ErrorCatcher up in the hierarchy will deal with it.
         */
		void propagate();

        const std::vector<Error> & errors() const { return mErrors; }

        const ErrorCatcher * child() const { return mChild.get(); }

	private:
		friend class ErrorReporter;
        ErrorCatcher & operator =(const ErrorCatcher & inErrorCatcher);

        /**
         * Log the error message using the callback set on the ErrorReporter.
         */
        void log();
        
        void push(const Error & inError);

        void setChild(const ErrorCatcher * inErrorCatcher);

        bool mOwns;
        std::vector<Error> mErrors;
        boost::shared_ptr<ErrorCatcher> mChild;
		bool mPropagate;
        bool mDisableLogging;
	};


	/**
	 * ErrorReporter serves as a global stack for ErrorCatcher objects.
	 */
	class ErrorReporter
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
		static ErrorReporter & Instance();
		
		/**
		 * Finalize should be called before shutting down your program.
		 */
		static void Finalize();

        typedef boost::function<void(const std::string &)> LogFunction;

        void setLogger(const LogFunction & inLogFunction);

		/**
		 * You can use this method to report an error, however
		 * the ReportError functions below are more convenient.
		 * NOTE: Calling this will not cause an exception to be thrown, so
		 *       program flow will not be altered. If you want to return to
		 *       the caller you still have to write a return statement.
		 */
		void reportError(const Error & inError);

	private:
		friend class ErrorCatcher;

        ErrorReporter();

        ~ErrorReporter();

		void push(ErrorCatcher * inError);

		void pop(ErrorCatcher * inError);

        void log(ErrorCatcher * inError);

		std::stack<ErrorCatcher*> mStack;
        LogFunction mLogFunction;
		static ErrorReporter * sInstance;
	};


	/**
	 * ReportError and its overloads are shorter versions 
	 * for ErrorReporter::Instance().reportError(..) 
	 * NOTE: These functions don't throw an actual C++ exception. They only
	 *       notify the nearest ErrorCatcher that an error has occured. So
	 *       you still need to write the return statement (if returning
	 *       is desired, of course).
	 */
	void ReportError(int inErrorCode, const std::string & inErrorMessage);


	/**
	 * ReportError by only passing a message. Default error code will be used (FAILED).
	 */
	void ReportError(const std::string & inErrorMessage);


	/**
	 * ReportError by only passing an error code.
	 */
	void ReportError(int inErrorCode);


} // namespace Utils


#endif // ERRORSTACK_H
