#include <iostream>
#include <chrono>


using Clock = std::chrono::system_clock;


void run(int core_id)
{
    // TODO: pin thread to core
    (void)core_id;


    auto start_time = Clock::now();
    auto next_snapshot_time = start_time + std::chrono::seconds(1);

    uint64_t total_duration_ns{0};
    uint64_t num_samples = 0;
    uint64_t max_sample = 0;
    uint64_t min_sample = -1;

    Clock::time_point t1 = Clock::now();

    for (;;)
    {
        Clock::time_point t2 = Clock::now();
        uint64_t sample = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        t1 = t2;

        total_duration_ns += sample;

        if (sample > max_sample)
        {
            max_sample = sample;
        }
        else if (sample < min_sample)
        {
            min_sample = sample;
        }
        num_samples++;

        if (t1 >= next_snapshot_time)
        {
            std::cout
                << "num_samples=" << num_samples
                << " total_duration_ns=" << total_duration_ns
                << " average_sample_duration=" << (num_samples ? total_duration_ns / num_samples : 0)
                << " max_sample=" << max_sample
                << " min_sample=" << min_sample
                << std::endl;

            next_snapshot_time += std::chrono::seconds(1);
            total_duration_ns = 0;
            num_samples = 0;
            max_sample = 0;
            min_sample = -1;
        }
    }
}



int main()
{
    run(0);
}
