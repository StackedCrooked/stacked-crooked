#include <chrono>
#include <atomic>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <vector>


using Clock = std::chrono::system_clock;
using Task = std::function<void()>;


struct ScheduledTask
{
    ScheduledTask(Clock::time_point tp, Task&& task) :
        mScheduledTime(tp),
        mTask(std::move(task))
    {
    }

    Clock::time_point mScheduledTime;
    Task mTask;
};


struct Timer
{
    void post(ScheduledTask task, Clock::time_point current_time)
    {
        using namespace std::chrono_literals;

        auto delay = current_time - task.mScheduledTime;
        if (delay > 1ms)
        {

        }
    }
    std::vector<ScheduledTask> mTasks_1s;
    std::vector<ScheduledTask> mTasks_100ms;
    std::vector<ScheduledTask> mTasks_10ms;
    std::vector<ScheduledTask> mTasks_1ms;
    std::vector<ScheduledTask> mTasks_100us;
    std::vector<ScheduledTask> mTasks_10us;
};


int main()
{
}
