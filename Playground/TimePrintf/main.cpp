#include "Poco/Stopwatch.h"
#include "Poco/Types.h"
#include <iostream>

static const long long cMicrosecond = 1000 * 1000;

int main()
{
	Poco::Stopwatch stopwatch;
	stopwatch.start();
	
	std::string message(75, '=');
	long long cIterationCount = 100000;
	for (size_t idx = 0; idx < cIterationCount; idx++)
	{
		std::cout << idx << message << std::endl;
	}

	stopwatch.stop();

	std::cout << "Number of iterations: " << cIterationCount << std::endl;

	long long elapsed = stopwatch.elapsed();
	std::cout << "Elapsed microseconds: " << elapsed  << std::endl;

	double elapsedSeconds = double(elapsed) / double(cMicrosecond);
	std::cout << "Elapsed seconds: " << elapsedSeconds << std::endl;


	std::cout << "Number of print statements per second: " << int(0.5 + (double(cIterationCount) / elapsedSeconds)) << std::endl;
	std::cout << "Duration of one print statement: " << (double(1000) * elapsedSeconds) / cIterationCount << " ms (milliseconds)" << std::endl;


	//double duration = double(elapsed) / (double(cIterationCount) * double(cMicrosecond));
	//std::cout << "Duration: " << duration << std::endl;
	return 0;
}

