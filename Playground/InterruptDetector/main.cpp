#include <boost/lockfree/spsc_queue.hpp>
#include <chrono>
#include <deque>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>


using Clock = std::chrono::system_clock;


struct Snapshot
{
    void add(uint64_t sample)
    {
        num_samples++;
        sum_samples += sample;

        if (sample > max_sample)
        {
            max_sample = sample;
        }
    }

    void clear()
    {
        num_samples = 0;
        max_sample = 0;
        sum_samples = 0;
    }

    uint64_t num_samples = 0;
    uint64_t sum_samples = 0;
    uint64_t max_sample = 0;
};


struct ThreadRunner
{
    explicit ThreadRunner(int core_id) :
        mCoreId(core_id),
        mThread()
    {
    }

    ThreadRunner(const ThreadRunner&) = default;
    ThreadRunner& operator=(const ThreadRunner&) = default;

    ThreadRunner(ThreadRunner&&) noexcept = default;
    ThreadRunner& operator=(ThreadRunner&&) noexcept  = default;

    ~ThreadRunner()
    {
        if (mThread.joinable())
        {
            mThread.join();
        }
    }

    void start()
    {
        mThread = std::thread([this]{
            run_thread();
        });
    }

    int getCoreId() const
    {
        return mCoreId;
    }

    bool consume(Snapshot& snapshot)
    {
        return mSnapshots.pop(&snapshot, 1);
    }

private:
    void run_thread()
    {
        {
            std::cout << "Pinning thread to core " << mCoreId << std::endl;
            auto cpuset = cpu_set_t();
            CPU_SET(mCoreId, &cpuset);
            pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
        }

        auto start_time = Clock::now();
        auto next_snapshot_time = start_time + std::chrono::seconds(1);

        Snapshot snapshot;

        auto iteration_counter = 0ul;

        for (;;)
        {
            Clock::time_point t1 = Clock::now();

            for (;;)
            {
                Clock::time_point t2 = Clock::now();

                if (t1 < next_snapshot_time)
                {
                    uint64_t sample = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
                    snapshot.add(sample);
                    t1 = t2;
                }
                else
                {
                    break;
                }
            }

            // The first iteration is always slow, so treat is as a warmup iteration and don't
            // print its result.
            if (iteration_counter++ > 0)
            {
                std::cout
                    << "num_samples=" << snapshot.num_samples
                    << " sum_samples=" << snapshot.sum_samples
                    << " average_sample_duration=" << (snapshot.num_samples ? snapshot.sum_samples / snapshot.num_samples : 0)
                    << " max _sample=" << snapshot.max_sample
                    << std::endl;
            }

            next_snapshot_time += std::chrono::seconds(1);
            snapshot.clear();
        }
    }

    boost::lockfree::spsc_queue<Snapshot, boost::lockfree::capacity<1>> mSnapshots;
    uint32_t mCoreId;
    std::thread mThread;
};



int main(int argc, char** argv)
{
    std::vector<std::string> args(argv, argv + argc);

    if (argc != 3)
    {
        std::string program_name = args[0];
        std::string basename = program_name.substr(program_name.rfind('/') + 1);
        std::cerr << "Usage:\n\t" << basename << " <core_id> <priority>\n";
        std::cerr << "\nExample:\n\t" << basename << " 1 SCHED_RR\n";
        std::cerr << "\nExample:\n\t" << basename << " 1 SCHED_OTHER\n";
        std::cerr << "\nNote: SCHED_OTHER is the default Linux scheduling policy.\n";
        return 1;
    }

    std::cout << "Settings:\n";


    uint32_t core_id = std::stoi(argv[1]);

    std::cout << "- Core id: " << core_id << std::endl;

    if (args[2] == "SCHED_RR")
    {
        std::cout << "- Scheduling priority: SCHED_RR\n";
        auto param = sched_param();
        int policy = 0;
        pthread_getschedparam(pthread_self(), &policy, &param);
        policy = SCHED_RR;
        param.sched_priority = sched_get_priority_min(policy);
        pthread_setschedparam(pthread_self(), policy, &param);
    }
    else if (args[2] == "SCHED_OTHER")
    {
        std::cout << "- Scheduling priority: SCHED_OTHER\n";
    }
    else
    {
        throw std::runtime_error("Invalid scheduling priority: " + args[2]);
    }

    std::cout << std::endl;


    std::deque<ThreadRunner> runners;
    runners.emplace_back(core_id);

    for (ThreadRunner& runner : runners)
    {
        Snapshot snapshot;
        if (runner.consume(snapshot))
        {
            std::cout
                << "CoreId: " << runner.getCoreId()
                << " num_samples=" << snapshot.num_samples
                << " sum_samples=" << snapshot.sum_samples
                << " average_sample_duration=" << (snapshot.num_samples ? snapshot.sum_samples / snapshot.num_samples : 0)
                << " max _sample=" << snapshot.max_sample
                << std::endl;

        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

}
