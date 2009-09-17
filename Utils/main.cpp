#include "ErrorStack.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <assert.h>


using namespace CppToys;


/**
 * Some pseudo error codes
 */
enum
{
	NO_ERROR,
	OPEN_FILE_FAILED,
	READ_FILE_FAILED,
	PROCESS_FILE_FAILED,
	WRITE_FILE_FAILED
};


void runSelfTest()
{
	class Formatter
	{
	public:
		Formatter(const std::string & inMessage)
		{
			std::cout << inMessage << std::setw(60 - inMessage.size()) << std::setfill('.');
		}
		~Formatter()
		{
			std::cout << "Done.\n";
		}
	};

	{
		Formatter f("Basic ErrorCatcher");
		ErrorCatcher se;
		assert(!se.hasCaught());
		ThrowError(OPEN_FILE_FAILED);
		assert(se.hasCaught());
		assert(se.code() == OPEN_FILE_FAILED);
	}

	{
		Formatter f("Nested ErrorCatcher without propagation");
		ErrorCatcher se1;
		ThrowError(OPEN_FILE_FAILED);
		{
			ErrorCatcher se2;
			ThrowError(READ_FILE_FAILED);
			assert(se2.hasCaught());
			assert(se2.code() == READ_FILE_FAILED);
		}
		assert(se1.code() == OPEN_FILE_FAILED);
	}

	{
		Formatter f("Nested ErrorCatcher with propagation");
		ErrorCatcher se1;
		ThrowError(READ_FILE_FAILED);
		{
			ErrorCatcher se2;
			ThrowError(PROCESS_FILE_FAILED);
			se2.rethrow();
		}
		assert (se1.code() == PROCESS_FILE_FAILED);
	}

	{
		Formatter f("Without ErrorCatcher");
		ThrowError(OPEN_FILE_FAILED);
		const Error & lastError = ErrorStack::Instance().lastError();
		assert(lastError.code() == OPEN_FILE_FAILED);
	}
}


float divideBy(float a, float b)
{
	if (b == 0)
	{
		ThrowError("You can't divide by zero.");
		return 0.0; // still need to return something
	}

	return a/b;
}

void getExifDataFromPhoto(const std::string & inFilePath, std::string & outExifData)
{

	if ("FileNotFound")
	{
		ThrowError("File not found.");
		return;
	}

	if ("NoExifDataFound")
	{
		ThrowError("No exif data found.");
		return;
	}

	// process exif data..
}


void runSamples()
{
	// Division by zero sample
	{
		ErrorCatcher errorCatcher;
		float a = divideBy(1, 0);
		if (errorCatcher.hasCaught())
		{
			std::cout << "Division failed. Reason: " << errorCatcher.message() << std::endl;
		}
	}	

	// Exif data sample
	{
		ErrorCatcher errorCatcher;
		std::string exifData;
		getExifDataFromPhoto("non-existing-file", exifData);
		if (errorCatcher.hasCaught())
		{
			std::cout << "Read exif data failed. Reason: " << errorCatcher.message() << std::endl;
		}		
	}
}
 

int main()
{
	ErrorStack::Initialize();
	
	runSelfTest();

	runSamples();	

	ErrorStack::Finalize();

	// To stop Visual Studio from closing the output window...
	std::cout << "\nPress ENTER to quit.";
	getchar();
	return 0;
}
