#include "Threading.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <vector>


struct PosixMutex : boost::noncopyable
{
    PosixMutex() { pthread_mutex_init(&mMutex, NULL); }

    ~PosixMutex() { pthread_mutex_destroy(&mMutex); }

    pthread_mutex_t & getMutex() { return mMutex; }

    const pthread_mutex_t & getMutex() const { return mMutex; }

    void lock() { pthread_mutex_lock(&mMutex); }

    void unlock() { pthread_mutex_unlock(&mMutex); }

    pthread_mutex_t mMutex;
};


typedef GenericThreading<PosixMutex> Threading;


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
        setQuitFlag();
    }

    bool setQuitFlag()
    {
        Threading::ScopedLock lock(mQuitFlagMutex);
        mQuitFlag = true;
    }

    bool getQuitFlag() const
    {
        Threading::ScopedLock lock(mQuitFlagMutex);
        return mQuitFlag;
    }

    // Print the entire vector
    void print()
    {
        Threading::ScopedAccessor<Characters> accessor(mCharacters);
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
        while (!getQuitFlag())
        {
            // Create an atomic scope
            Threading::ScopedAccessor<Characters> accessor(mCharacters);
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
        while (!getQuitFlag())
        {
            // Create an atomic scope
            Threading::ScopedAccessor<Characters> accessor(mCharacters);
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
    Threading::ThreadSafe<Characters> mCharacters;

    boost::scoped_ptr<boost::thread> mControllerThread;
    boost::scoped_ptr<boost::thread> mAppendDigitsThread;
    boost::scoped_ptr<boost::thread> mAppendLettersThread;

    bool mQuitFlag;
    mutable Threading::Mutex mQuitFlagMutex;
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

