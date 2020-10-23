#include <boost/lockfree/spsc_queue.hpp>
#include <chrono>
#include <condition_variable>
#include <csignal>
#include <cstring>
#include <deque>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>


using Clock = std::chrono::system_clock;


static void pin_current_thread_to_core(int core_id)
{
    auto cpuset = cpu_set_t();
    CPU_SET(core_id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
}


static void set_current_thread_name(const std::string& threadname)
{
    pthread_setname_np(pthread_self(), threadname.c_str());
}


// Helper function for registration of a signal handler.
// This handles SIGTERM and SIGINT. (SIGKILL cannot be handled)
// You must pass a function pointer of signature `void(int)`
typedef void (*signal_handler_t)(int);
static void add_signal_handler(signal_handler_t signal_handler)
{
    typedef struct sigaction sigaction_t;
    auto action = sigaction_t();
    action.sa_handler = signal_handler;
    action.sa_flags = SA_RESETHAND;
    sigaction(SIGTERM, &action, 0);
    sigaction(SIGINT, &action, 0);
}


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

    bool is_snapshot_available() const
    {
        return mSnapshots.read_available();
    }

    bool consume(Snapshot& snapshot)
    {
        return mSnapshots.pop(&snapshot, 1);
    }

private:
    void run_thread()
    {
        set_current_thread_name("CPU" + std::to_string(mCoreId));

        std::cout << "ThreadRunner::run_thread: pinning thread to core " << mCoreId << "\n";
        pin_current_thread_to_core(mCoreId);

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
//                std::cout
//                    << "num_samples=" << snapshot.num_samples
//                    << " sum_samples=" << snapshot.sum_samples
//                    << " average_sample_duration=" << (snapshot.num_samples ? snapshot.sum_samples / snapshot.num_samples : 0)
//                    << " max _sample=" << snapshot.max_sample
//                    << std::endl;
                mSnapshots.push(snapshot);
            }

            next_snapshot_time += std::chrono::seconds(1);
            snapshot.clear();
        }
    }

    boost::lockfree::spsc_queue<Snapshot, boost::lockfree::capacity<1>> mSnapshots;
    uint32_t mCoreId;
    std::thread mThread;
};


/// Parses a range parameter.
/// Examples:
///     "1-5" will return { 1, 5 } as an std::pair
///
/// Single number is treated as a range of one element:
///     "3" will return { 3, 3 } as an std::pair
///
std::pair<int, int> ParseRange(const std::string& str)
{
    auto dash_position = str.find('-');
    if (dash_position == std::string::npos)
    {
        int core_id = std::stoi(str);
        return { core_id, core_id };
    }

    auto from = std::stoi(str.substr(0, dash_position));
    auto to = std::stoi(str.substr(dash_position + 1));

    return { from, to };
}

std::vector<int> GetCoreIds(const std::string& str)
{
    std::pair<int, int> core_id_range = ParseRange(str);
    assert(core_id_range.first <= core_id_range.second);

    std::vector<int> result;
    result.reserve(core_id_range.second - core_id_range.first + 1);

    for (auto core_id = core_id_range.first; core_id <= core_id_range.second; ++core_id)
    {
        result.push_back(core_id);
    }

    return result;
}



int main(int argc, char** argv)
{
    std::vector<std::string> args(argv, argv + argc);

    if (argc != 3)
    {
        std::string program_name = args[0];
        std::string basename = program_name.substr(program_name.rfind('/') + 1);
        std::cerr << "Usage:\n\t" << basename << " <core_id_range> <priority>\n";
        std::cerr << "\nExample:\n\t" << basename << " 1-12 SCHED_RR\n";
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

    // Trigger shutdown on SIGINT
    std::cout << "Registring signal handler.\n";
    add_signal_handler([](int signal) {
        std::cerr << "\nReceived signal " << signal << "(" << strsignal(signal) << ")" << std::endl;
        std::exit(0);
    });


    /// The main thread is on core zero.
    pin_current_thread_to_core(0);

    std::deque<ThreadRunner> runners;

    for (int core_id : GetCoreIds(args[1]))
    {
        if (core_id == 0)
        {
            throw std::runtime_error("Core 0 not allowed.");
        }

        std::cout << "Starting thread for core " << core_id << std::endl;
        runners.emplace_back(core_id);
    }

    for (ThreadRunner& runner : runners)
    {
        runner.start();
    }

    /// WAIT UNTIL THREADS ARE STARTED
    /// FIXME: USE NOTIFICATION SYSTEM HERE!!!
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::cout << "(Press Ctrl-C to exit)\n";

    std::cout << "\n";

    enum { column_width = 7 };

    for (ThreadRunner& runner : runners)
    {
        std::cout << std::setw(column_width) << std::right << ("CPU" + std::to_string(runner.getCoreId()));
    }

    std::cout << '\n';

    for (;;)
    {
        bool all_snapshots_available = true;

        for (ThreadRunner& runner : runners)
        {
            if (!runner.is_snapshot_available())
            {
                all_snapshots_available = false;
                break;
            }
            break;
        }

        if (!all_snapshots_available)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        std::vector<Snapshot> snapshots;
        snapshots.reserve(runners.size());

        for (ThreadRunner& runner : runners)
        {
            Snapshot snapshot;
            if (runner.consume(snapshot))
            {
                snapshots.push_back(snapshot);
            }
        }

        for (const Snapshot& snapshot : snapshots)
        {
            std::cout << std::setw(column_width) << std::right << snapshot.max_sample;
        }

        std::cout << std::endl;
    }
}
