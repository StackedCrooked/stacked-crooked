#include "ErrorHandler.h"
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
	ErrorReporter::Instance().reportError(OPEN_FILE_FAILED);
}

void readFile()
{
	ScopedError scopedError;
	
	openFile();
	if (scopedError.isError())
	{
		// propagate because want the caller to handle the error
		scopedError.propagate();
		return;
	}

	bool readFailed = true;
	if (readFailed)
	{
		ErrorReporter::Instance().reportError(READ_FILE_FAILED);

		// propagation needed because we reported to local errorHandle
		// and we want the error to be handled by its parent
		scopedError.propagate(); 
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
	ScopedError scopedError;
	readFile();
	if (scopedError.isError())
	{
		log(scopedError.errorCode(), "Failed to read the file");
		return;
	}

	processFile();
	if (scopedError.isError())
	{
		log(scopedError.errorCode(), "Failed to process the file.");
		return;
	}

	writeFile();
	if (scopedError.isError())
	{
		log(scopedError.errorCode(), "Failed to write the file.");
		return;
	}
}


void test()
{
	// Basic usage
	{
		ErrorReporter::Instance().reportError(OPEN_FILE_FAILED);
		int lastError = ErrorReporter::Instance().lastError();
		assert(lastError == OPEN_FILE_FAILED);
	}


	// test ScopedError
	{
		ScopedError se;
		assert(!se.isError());
		ErrorReporter::Instance().reportError(OPEN_FILE_FAILED);
		assert(se.isError());
		assert(se.errorCode() == OPEN_FILE_FAILED);
	}

	// test nested ScopedError without propagation
	{
		ScopedError se1;
		{
			ScopedError se2;
			ErrorReporter::Instance().reportError(READ_FILE_FAILED);
			assert(se2.isError());
			assert(se2.errorCode() == READ_FILE_FAILED);
		}
		assert(!se1.isError());
	}


	// test nested ScopedError with propagation
	{
		ScopedError se1;
		{
			ScopedError se2;
			ErrorReporter::Instance().reportError(PROCESS_FILE_FAILED);
			se2.propagate();
		}
		assert (se1.isError());
		assert (se1.errorCode() == PROCESS_FILE_FAILED);
	}
}


int main()
{
	ErrorReporter::CreateInstance();
	
	test();
	testErrorHandler();

	ErrorReporter::DestroyInstance();

	std::cout << "Press ENTER to quit.";
	getchar();
	return 0;
}
