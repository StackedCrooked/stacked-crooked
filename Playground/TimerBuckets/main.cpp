#include <chrono>
#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <vector>


using Clock = std::chrono::system_clock;
using Task = std::function<void()>;

//typedef duration<long long,         nano> nanoseconds;
//typedef duration<long long,        micro> microseconds;
//typedef duration<long long,        milli> milliseconds;
//typedef duration<long long              > seconds;
//typedef duration<     long, ratio<  60> > minutes;
//typedef duration<     long, ratio<3600> > hours;


//typedef ratio<1LL, 1000000000000000000LL> atto;
//typedef ratio<1LL,    1000000000000000LL> femto;
//typedef ratio<1LL,       1000000000000LL> pico;
//typedef ratio<1LL,          1000000000LL> nano;
//typedef ratio<1LL,             1000000LL> micro;
//typedef ratio<1LL,                1000LL> milli;
//typedef ratio<1LL,                 100LL> centi;
//typedef ratio<1LL,                  10LL> deci;
//typedef ratio<                 10LL, 1LL> deca;
//typedef ratio<                100LL, 1LL> hecto;
//typedef ratio<               1000LL, 1LL> kilo;
//typedef ratio<            1000000LL, 1LL> mega;
//typedef ratio<         1000000000LL, 1LL> giga;
//typedef ratio<      1000000000000LL, 1LL> tera;
//typedef ratio<   1000000000000000LL, 1LL> peta;
//typedef ratio<1000000000000000000LL, 1LL> exa;

// DecaMicro means "10 micro"

namespace Detail {
using DecaMicro  = std::ratio<std::deca::num, std::micro::den>;
using HectoMicro = std::ratio<std::hecto::num, std::micro::den>;
}

using Seconds      = std::chrono::seconds;
using Milliseconds = std::chrono::milliseconds;
using Microseconds = std::chrono::microseconds;
using Nanoseconds  = std::chrono::nanoseconds;


using DecaMicroseconds  =  std::chrono::duration<Microseconds::rep, Detail::DecaMicro>;
using HectoMicroseconds = std::chrono::duration<Microseconds::rep, Detail::HectoMicro>;


static_assert(DecaMicroseconds(1)  == Microseconds(10), "");
static_assert(HectoMicroseconds(1) == Microseconds(100), "");


struct ScheduledTask
{
    ScheduledTask(Clock::time_point tp, Task&& task) :
        mScheduledTime(tp),
        mTask(std::move(task))
    {
    }

    Clock::time_point mScheduledTime;
    int mRemainingTurns = 0;
    Task mTask;
};


enum { num_buckets = 10 };




struct HashedTimerWheel
{
    void add(Task task, int delay)
    {
        auto index = delay % num_buckets;
        auto count = delay / num_buckets;

        Bucket& bucket = mBuckets[(mActiveIndex + index) % 10];

        Entry entry;
        entry.mCount = count;
        entry.mTask = task;
        bucket.mEntries.push_back(entry);
    }


    struct Entry
    {
        int mCount = 0;
        Task mTask;
    };


    struct Bucket
    {
        void run_tasks()
        {
            std::swap(mEntries, mEntries2);


            for (Entry& entry : mEntries2)
            {
                if (entry.mCount-- == 0)
                {
                    entry.mTask();
                }
                else
                {
                    std::cout << "Execute later " << entry.mCount << "\n";
                    mEntries3.push_back(std::move(entry));
                }
            }

            mEntries2.clear();


            // Check for entries inserted while running the timers.
            if (!mEntries.empty())
            {
                mEntries3.insert(mEntries3.end(), mEntries.begin(), mEntries.end());
                mEntries.clear();
            }

            std::swap(mEntries, mEntries3);
        }

        std::vector<Entry> mEntries;
        std::vector<Entry> mEntries2;
        std::vector<Entry> mEntries3;
    };


    void tick()
    {
        mBuckets[mActiveIndex].run_tasks();

        if (++mActiveIndex == num_buckets)
        {
            mActiveIndex = 0;
        }
    }

    int mActiveIndex = 0;
    Bucket mBuckets[10];


};



int main()
{
    HashedTimerWheel table;
    table.add([]{ std::cout << "1" << std::endl; }, 1);
    table.add([]{ std::cout << "13" << std::endl; }, 13);
    table.add([]{ std::cout << "31" << std::endl; }, 31);

    for (auto i = 0; i != 33; ++i)
    {
        std::cout << "i=" << i << std::endl;
        table.tick();
    }
}
