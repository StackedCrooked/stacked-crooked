#include "Threading.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <string>
#include <vector>


class Tester
{
public:
    Tester() :
        mCharacters(),
        mAppendDigitsThread(),
        mAppendLettersThread(),
        mQuitFlag(false),
        mQuitFlagMutex()
    {
    }

    ~Tester()
    {
        setQuitFlag();
        mAppendDigitsThread->join();
        mAppendLettersThread->join();
        mControllerThread->join();

    }

    void start()
    {
        mControllerThread.reset(new boost::thread(boost::bind(&Tester::startImpl, this)));
    }

    void stop()
    {
        setQuitFlag();
    }

    bool setQuitFlag()
    {
        ScopedLock lock(mQuitFlagMutex);
        mQuitFlag = true;
    }

    bool getQuitFlag() const
    {
        ScopedLock lock(mQuitFlagMutex);
        return mQuitFlag;
    }

    // Print the entire vector
    void print()
    {
        ATOMIC_SCOPE(Characters, mCharacters)
        {
            for (std::size_t idx = 0; idx < mCharacters.size(); ++idx)
            {
                std::cout << mCharacters[idx];
            }
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
        while (!getQuitFlag())
        {
            ATOMIC_SCOPE(Characters, mCharacters)
            {
                for (char c = '0'; c <= '9'; ++c)
                {
                    mCharacters.push_back(c);
                }
                mCharacters.push_back('\n');
            }
        }
    }

    // Continuously append digits ABCDEFGHIJKLMNOPQRSTUVWXYZ to the vector followed by a newline.
    void appendLetters()
    {
        while (!getQuitFlag())
        {
            ATOMIC_SCOPE(Characters, mCharacters)
            {
                for (char c = 'A'; c <= 'Z'; ++c)
                {
                    mCharacters.push_back(c);
                }
                mCharacters.push_back('\n');
            }
        }
    }

    typedef std::vector<char> Characters;
    ThreadSafe<Characters> mCharacters;

    boost::scoped_ptr<boost::thread> mControllerThread;
    boost::scoped_ptr<boost::thread> mAppendDigitsThread;
    boost::scoped_ptr<boost::thread> mAppendLettersThread;

    bool mQuitFlag;
    mutable Mutex mQuitFlagMutex;
};


int main()
{
    Tester tester;

    // Start the worker threads
    tester.start();

    // Sleep 500 ms
    boost::xtime duration = {0, 10 * 1000 * 1000};
    boost::this_thread::sleep(duration);

    // Stop all threads.
    tester.stop();

    // Print the results
    //tester.print();

    std::cout << "Everything is OK." << std::endl;
    return 0;
}
