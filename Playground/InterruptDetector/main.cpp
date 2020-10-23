#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


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


void run(int core_id)
{
    {
        std::cout << "Pinning thread to core " << core_id << std::endl;
        auto cpuset = cpu_set_t();
        CPU_SET(core_id, &cpuset);
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
                << " max_sample=" << snapshot.max_sample
                << std::endl;
        }

        next_snapshot_time += std::chrono::seconds(1);
        snapshot.clear();
    }
}



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

    run(core_id);
}
