#include "Threading.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <vector>


using namespace Threading;




class Tester
{
public:
	Tester() :
		mCharacters(),
		mAppendDigitsThread(),
		mAppendLettersThread()
	{
	}

	void start()
	{
		mControllerThread.reset(new boost::thread(boost::bind(&Tester::startImpl, this)));
	}

	void stop()
	{
		mControllerThread->interrupt();
	}

	void print()
	{
		ScopedAccessor<Characters, PosixMutex> accessor(mCharacters);
		Characters & characters = accessor.get();
		for (std::size_t idx = 0; idx < characters.size(); ++idx)
		{
			std::cout << characters[idx];
		}
	}


	void startImpl()
	{
		mAppendDigitsThread.reset(new boost::thread(boost::bind(&Tester::appendLetters, this)));
		mAppendLettersThread.reset(new boost::thread(boost::bind(&Tester::appendDigits, this)));
	}

	void appendDigits()
	{
		while (true)
		{
            ScopedAccessor<Characters, PosixMutex> accessor(mCharacters);
            Characters & characters = accessor.get();

			for (char c = '0'; c <= '9'; ++c)
			{
				characters.push_back(c);
			}
            characters.push_back('\n');
			boost::this_thread::interruption_point();
		}
	}

	void appendLetters()
	{
		while (true)
		{
            ScopedAccessor<Characters, PosixMutex> accessor(mCharacters);
            Characters & characters = accessor.get();

			for (char c = 'A'; c <= 'Z'; ++c)
			{
				characters.push_back(c);
			}
            characters.push_back('\n');
			boost::this_thread::interruption_point();
		}
	}

	typedef std::vector<char> Characters;
	ThreadSafe<Characters, PosixMutex> mCharacters;

	boost::scoped_ptr<boost::thread> mControllerThread;
	boost::scoped_ptr<boost::thread> mAppendDigitsThread;
	boost::scoped_ptr<boost::thread> mAppendLettersThread;
};


int main()
{
	std::cout << "Testing\n";
	Tester tester;
	std::cout << "Starting\n";
	tester.start();
	std::cout << "Sleep 1s\n";
	sleep(1);
	std::cout << "Stopping\n";
	tester.stop();
	std::cout << "Printing\n";
	tester.print();
	

    std::cout << "Everything went better than expected." << std::endl;
    return 0;
}
