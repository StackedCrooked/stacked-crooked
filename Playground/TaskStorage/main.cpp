#include <boost/lockfree/spsc_queue.hpp>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <array>
#include <string>
#include <vector>


struct Scheduler
{
    Scheduler()
    {
        for (auto i = 0; i != Capacity; ++i)
        {
            mQueue.push(i++);
        }
    }

    template<typename F>
    void post(F&& f) noexcept
    {
        mTasks.push_back(Create(std::forward<F>(f), *this));
    }

    void run() noexcept
    {
        for (TaskImplBase* taskImplBase : mTasks)
        {
            taskImplBase->call();
            if (taskImplBase->dispose(*this))
            {
                auto freed = mFreed;
                mFreed = (freed + 1) % Capacity;
                if (!mQueue.push(freed))
                {
                    throw std::runtime_error("Failed to free!");
                }
            }
        }
        mTasks.clear();
    }

private:
    enum { Capacity = 1024 };
    struct TaskImplBase;
    using Queue = boost::lockfree::spsc_queue<uint16_t, boost::lockfree::capacity<Capacity>>;

    using LocalStorage = typename std::aligned_storage<32, 32>::type;

    struct TaskImplBase
    {
        virtual ~TaskImplBase() { }
        virtual void call() = 0;
        virtual bool dispose(Scheduler& s) = 0;
    };

    template<typename F, bool HasLocalStorage>
    struct TaskImpl : TaskImplBase
    {
        TaskImpl(F f) : mF(std::move(f)) {}

        TaskImpl(const TaskImpl&) = delete;
        TaskImpl& operator=(const TaskImpl&) = delete;

        virtual ~TaskImpl() { }

        void call() override final
        {
            mF();
        }
        bool dispose(Scheduler& /*s*/) override final
        {
            if (HasLocalStorage)
            {
                this->~TaskImpl();
                return true;
            }
            else
            {
                delete this;
                return false;
            }
        }

        F mF;
    };

    template<typename F>
    static TaskImplBase* Create(F f, Scheduler& s)
    {
        uint16_t i = 0;
        if (sizeof(F) <= sizeof(LocalStorage))
        {
            if (!s.mQueue.pop(i))
            {
                s.run();
            }
            else
            {
                goto doit;
            }

            if (!s.mQueue.pop(i))
            {
            }
            else
            {
                doit:
                LocalStorage& local_storage = s.mLocalStorages[i];
                auto result = new (&local_storage) TaskImpl<F, true>(std::move(f));
                return result;
            }
        }
        return new TaskImpl<F, false>(std::move(f));
    }

    std::array<LocalStorage, Capacity> mLocalStorages;
    uint16_t mFreed = 0;
    Queue mQueue;
    std::vector<TaskImplBase*> mTasks;
};



int main()
{
    Scheduler s;

    for (auto i = 0; i != 200; ++i)
    {
        std::cout << "================ i=" << i << " ================" << std::endl;
        s.post([]{});

        std::array<char, 1024> big_array;
        s.post([big_array]{
            std::cout << "    big_array.size=" << sizeof(big_array) << std::endl;

        });


        std::string abc("abc");

        s.post([abc] {

            std::cout << "    abc=" << abc << std::endl;
        });

        std::array<std::string, 4> strings;
        for (auto& s : strings)
        {
            s = abc;
            std::cout << "    s=" << s << std::endl;
        }

        s.post([strings]{
            for (auto& s : strings)
            {
                std::cout << "    s[" << (&s - strings.data()) << "]=" << s << std::endl;
            }
        });
        s.run();
    }
}
