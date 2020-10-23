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
    // TODO: pin thread to core
    (void)core_id;


    auto start_time = Clock::now();
    auto next_snapshot_time = start_time + std::chrono::seconds(1);



    Snapshot snapshot;

    for (;;)
    {

        Clock::time_point t1 = Clock::now();

        for (;;)
        {
            Clock::time_point t2 = Clock::now();
            uint64_t sample = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
            t1 = t2;

            snapshot.add(sample);

            if (t1 >= next_snapshot_time)
            {
                std::cout
                    << "num_samples=" << snapshot.num_samples
                    << " sum_samples=" << snapshot.sum_samples
                    << " average_sample_duration=" << (snapshot.num_samples ? snapshot.sum_samples / snapshot.num_samples : 0)
                    << " max_sample=" << snapshot.max_sample
                    << std::endl;

                next_snapshot_time += std::chrono::seconds(1);
                snapshot.clear();
                break;
            }
        }
    }
}



int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::string program_name = argv[0];
        std::string basename = program_name.substr(program_name.rfind('/') + 1);
        std::cerr << "Usage: " << basename << " <core_id>\n";
        return 1;
    }

    uint32_t core_id = std::stoi(argv[1]);

    std::cout << "Running on core " << core_id << std::endl;

    run(core_id);
}
