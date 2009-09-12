#include "ErrorReporter.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <assert.h>


enum
{
	NO_ERROR,
	OPEN_FILE_FAILED,
	READ_FILE_FAILED,
	PROCESS_FILE_FAILED,
	WRITE_FILE_FAILED
};


void openFile()
{
	// suppose it fails
	ErrorReporter::Instance().throwError(OPEN_FILE_FAILED);
}

void readFile()
{
	ErrorCatcher errorCatcher;
	
	openFile();
	if (errorCatcher.caughtError())
	{
		// rethrow because want the caller to handle the error
		errorCatcher.rethrow();
		return;
	}

	// once the file is opened we want to read it
	// suppose that fails
	bool readFailed = true;
	if (readFailed)
	{
		ErrorReporter::Instance().throwError(READ_FILE_FAILED);

		// propagation needed because we reported to local errorHandle object
		// and we want the error to be handled by its parent
		errorCatcher.rethrow(); 
	}
}

void processFile()
{
	// process the file
}

void writeFile()
{
	// write the file
}

void log(int inErrorCode, const std::string & inMessage)
{
	std::string errorCodeName;
	switch (inErrorCode)
	{
		case NO_ERROR:
		{
			errorCodeName = "NO_ERROR";
			break;
		}
		case OPEN_FILE_FAILED:
		{
			errorCodeName = "OPEN_FILE_FAILED";
			break;
		}
		case READ_FILE_FAILED:
		{
			errorCodeName = "READ_FILE_FAILED";
			break;
		}
		case PROCESS_FILE_FAILED:
		{
			errorCodeName = "PROCESS_FILE_FAILED";
			break;
		}
		case WRITE_FILE_FAILED:
		{
			errorCodeName = "WRITE_FILE_FAILED";
			break;
		}
		default:
		{
			errorCodeName = "UNKNOWN_ERROR";
			break;
		}
	}
	
	std::cout << inMessage << " (" << errorCodeName << ")" << std::endl;
}

void testErrorHandler()
{
	ErrorCatcher errorCatcher;
	readFile();
	if (errorCatcher.caughtError())
	{
		log(errorCatcher.error().errorCode(), "Failed to read the file");
		return;
	}

	processFile();
	if (errorCatcher.caughtError())
	{
		log(errorCatcher.error().errorCode(), "Failed to process the file.");
		return;
	}

	writeFile();
	if (errorCatcher.caughtError())
	{
		log(errorCatcher.error().errorCode(), "Failed to write the file.");
		return;
	}
}

class TestInfo
{
public:
	TestInfo(const std::string & inMessage)
	{
		std::cout << inMessage << std::setw(60 - inMessage.size()) << std::setfill('.');
	}
	~TestInfo()
	{
		std::cout << "OK\n";
	}
};

void test()
{
	{
		TestInfo ti("Basic usage");
		ThrowError(OPEN_FILE_FAILED);
		const Error & lastError = ErrorReporter::Instance().lastError();
		assert(lastError.errorCode() == OPEN_FILE_FAILED);
	}

	{
		TestInfo ti("With ErrorCatcher");
		ErrorCatcher se;
		assert(!se.caughtError());
		ThrowError(OPEN_FILE_FAILED);
		assert(se.caughtError());
		assert(se.error().errorCode() == OPEN_FILE_FAILED);
	}

	{
		TestInfo ti("Nested ErrorCatcher without propagation");
		ErrorCatcher se1;
		ThrowError(OPEN_FILE_FAILED);
		{
			ErrorCatcher se2;
			ThrowError(READ_FILE_FAILED);
			assert(se2.caughtError());
			assert(se2.error().errorCode() == READ_FILE_FAILED);
		}
		assert(se1.error().errorCode() == OPEN_FILE_FAILED);
	}

	{
		TestInfo ti("Nested ErrorCatcher with propagation");
		ErrorCatcher se1;
		ThrowError(READ_FILE_FAILED);
		{
			ErrorCatcher se2;
			ThrowError(PROCESS_FILE_FAILED);
			se2.rethrow();
		}
		assert (se1.error().errorCode() == PROCESS_FILE_FAILED);
	}
}


int main()
{
	ErrorReporter::CreateInstance();
	
	test();

	ErrorReporter::DestroyInstance();

	std::cout << "\nPress ENTER to quit.";
	getchar();
	return 0;
}
