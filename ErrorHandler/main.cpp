#include "ErrorHandler.h"
#include <iostream>
#include <string>


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
	ReportError(OPEN_FILE_FAILED);
}

void readFile()
{
	ScopedErrorHandler errorHandler;
	
	openFile();
	if (errorHandler.hasError())
	{
		// propagate because want the caller to handle the error
		errorHandler.propagate();
		return;
	}

	bool readFailed = true;
	if (readFailed)
	{
		ReportError(READ_FILE_FAILED);

		// propagation needed because we reported to local errorHandle
		// and we want the error to be handled by its parent
		errorHandler.propagate(); 
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
	ScopedErrorHandler errorHandler;
	readFile();
	if (errorHandler.hasError())
	{
		log(errorHandler.errorCode(), "Failed to read the file");
		return;
	}

	processFile();
	if (errorHandler.hasError())
	{
		log(errorHandler.errorCode(), "Failed to process the file.");
		return;
	}

	writeFile();
	if (errorHandler.hasError())
	{
		log(errorHandler.errorCode(), "Failed to write the file.");
		return;
	}
}


int main()
{
	ErrorReporter::CreateInstance();
	
	testErrorHandler();

	ErrorReporter::DestroyInstance();

	std::cout << "Press ENTER to quit.";
	getchar();
	return 0;
}
