#ifndef ACTOR_H
#define ACTOR_H


#include <atomic>
#include <boost/chrono.hpp>
#include <boost/thread/future.hpp>


template<typename T>
class Actor
{
public:
    template<typename ...Args>
    Actor(Args && ...args) :
        obj(std::forward<Args>(args)...),
        started(false),
        quit(false),
        consumer_thread(std::bind(&Actor<T>::consume, this)),
        queue()
    {
    }

    ~Actor()
    {
        {
            quit = true;
        }
        consumer_thread.join();
    }

    Actor(Actor&&) = default;
    Actor& operator=(Actor&&) = default;

    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;

    template<typename Ret>
    boost::future<Ret> execute(const std::function<Ret(const T &)> & f)
    {
        auto promisePtr = std::make_shared< boost::promise<Ret> >();
        auto l = [this, f, promisePtr]() { promisePtr->set_value(f(obj)); };
        Task task(l);
        queue.push_back(task);
        auto fut = promisePtr->get_future();
        return fut;
    }

private:
    // type erased in order to allow various types of tasks
    typedef std::function<void()> Task;

    void consume()
    {
        while (!quit)
        {
            if (!queue.empty())
            {
                queue.front()();
                queue.pop_front();
            }
            boost::thread::this_thread::sleep_for(boost::chrono::milliseconds(50));
        }
    }

    T obj;
    std::atomic<bool> started, quit;
    boost::thread consumer_thread;
    std::deque<Task> queue; // lockless queue would be better
};


#endif // ACTOR_H
