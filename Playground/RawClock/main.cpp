#include <iostream>
#include <chrono>
#include <thread>



using SystemClock = std::chrono::system_clock;



struct RawClock
{
    using duration = std::chrono::nanoseconds;
    using rep = duration::rep;
    using period = duration::period;
    using time_point = std::chrono::time_point<RawClock, duration>;

    static_assert(RawClock::duration::min() < RawClock::duration::zero(), "a clock's minimum duration cannot be less than its epoch");

    static constexpr bool is_steady = false;

    static time_point now() noexcept
    {
        auto tp = timespec();
        clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
        return time_point(duration(std::chrono::seconds(tp.tv_sec) + std::chrono::nanoseconds(tp.tv_nsec)));
    }
};


static std::chrono::nanoseconds system_start_time = SystemClock::now().time_since_epoch();
static std::chrono::nanoseconds raw_start_time = RawClock::now().time_since_epoch();


int main()
{
    auto next_wakeup = SystemClock::now();

    for (auto i = 0; i != 1000 * 1000; ++i)
    {
        auto r = RawClock::now() - raw_start_time;
        auto s = SystemClock::now() - system_start_time;

        auto difference = s.time_since_epoch() - r.time_since_epoch();

        std::cout << "r=" << r.time_since_epoch().count() << " s=" << s.time_since_epoch().count() << " s-r=" << difference.count() << std::endl;

        std::this_thread::sleep_until(next_wakeup);
        next_wakeup += std::chrono::milliseconds(1000);
    }


}
