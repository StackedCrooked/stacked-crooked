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
    void post(F&& f)
    {
        //std::cout << "=> PUSHING TASK <= mFreed=" << mFreed << std::endl;
        mTasks.push_back(Create(std::forward<F>(f), *this));
    }

    void run()
    {
        std::cout << "Running " << mTasks.size() << " tasks mFreed=" << mFreed << std::endl;
        for (TaskImplBase* taskImplBase : mTasks)
        {
            taskImplBase->call();
            if (taskImplBase->dispose(*this))
            {
                if (!mQueue.push(mFreed++ % Capacity))
                {
                    std::cout << "PUSH FAILED! mFreed=" << mFreed << std::endl;
                }
                std::cout << "mFreed=" << mFreed << std::endl;
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
            //std::cout << "Calling this: " << typeid(*this).name() << std::endl;
            mF();
        }
        bool dispose(Scheduler& /*s*/) override final
        {
            if (HasLocalStorage)
            {
                //std::cout << "In-place destruction of this: " << typeid(*this).name() << std::endl;
                this->~TaskImpl();
                return true;
            }
            else
            {
                //std::cout << "Deleting this: " << typeid(*this).name() << std::endl;
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

            if (s.mQueue.pop(i))
            {
                //std::cout << "YES TO QUEUE => USE NEW " << std::endl;
                LocalStorage& local_storage = s.mLocalStorages[i];
                auto result = new (&local_storage) TaskImpl<F, true>(std::move(f));
                return result;
            }

        }
        //std::cout << "FAILED TO QUEUE => USE NEW " << std::endl;
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

    for (auto i = 0; i != 4000; ++i)
    {
        s.post([]{});

        std::array<char, 1024> big_array;
        s.post([big_array]{});


        std::string abc("abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc");

        s.post([abc] {
            //std::cout << abc << std::endl;
        });

        std::array<std::string, 4> strings;
        for (auto& s : strings)
        {
            s = abc;
        }

        s.post([strings]{
            for (auto& s : strings)
            {
                //std::cout << "s=" << s << std::endl;
                (void)s;
            }
        });


    }
    s.run();
}
