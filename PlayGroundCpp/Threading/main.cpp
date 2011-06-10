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
        // Interrput the controller thread, which will result in the interruption of the worker threads.
        mControllerThread->interrupt();
    }

    // Print the entire vector
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
        // Start the two worker threads
        mAppendDigitsThread.reset(new boost::thread(boost::bind(&Tester::appendLetters, this)));
        mAppendLettersThread.reset(new boost::thread(boost::bind(&Tester::appendDigits, this)));
    }


    // Continuously append digits 0123456789 to the vector followed by a newline.
    void appendDigits()
    {
        while (true)
        {
            // Create an atomic scope
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

    // Continuously append digits ABCDEFGHIJKLMNOPQRSTUVWXYZ to the vector followed by a newline.
    void appendLetters()
    {
        while (true)
        {
            // Create an atomic scope
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
    Tester tester;

    // Start the worker threads
    tester.start();

    // Sleep 500 ms
    boost::xtime duration = {0, 500 * 1000 * 1000};
    boost::this_thread::sleep(duration);

    // Interrupt the treads
    tester.stop();

    // Print the results
    tester.print();

    std::cout << "Everything went better than expected." << std::endl;
    return 0;
}

