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
    }

    template<typename F>
    void post(F&& f) noexcept
    {
        mTasks.push_back(Create(std::forward<F>(f), *this));
    }

    void run() noexcept
    {
        std::cout << "+++++++++++++++++ run mTasks.size=" << mTasks.size() << std::endl;
        for (TaskImplBase* taskImplBase : mTasks)
        {
            taskImplBase->call();
            if (taskImplBase->dispose(*this))
            {
                mFreed++;
            }
        }
        mTasks.clear();
    }

private:
    enum { Capacity = 64 };
    struct TaskImplBase;

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
        if (sizeof(F) <= sizeof(LocalStorage))
        {
            auto freed = s.mFreed.load();
            auto alloc = s.mAllocated.load();
            if (alloc - freed == Capacity)
            {
                s.run();
                assert(s.mFreed > freed);
                assert(s.mAllocated == alloc);
            }

            LocalStorage& local_storage = s.mLocalStorages[s.mAllocated++ % Capacity];

            auto result = new (&local_storage) TaskImpl<F, true>(std::move(f));
            return result;
        }
        return new TaskImpl<F, false>(std::move(f));
    }

    std::array<LocalStorage, Capacity> mLocalStorages;
    std::atomic<uint16_t> mAllocated{0};
    std::atomic<uint16_t> mFreed{0};
    std::vector<TaskImplBase*> mTasks;
};



int main()
{
    Scheduler s;

    for (auto i = 0; i != 100; ++i)
    {
        s.post([]{
            std::cout << "Hi!" << std::endl;
        });

        auto big_array = std::array<char, 1024>();
        s.post([big_array]{
            std::cout << "big_array[middle]=" << big_array[big_array.size() / 2] << std::endl;

        });

        std::vector<std::string> vec;
        for (auto i = 0; i != 1024; ++i)
        {

            vec.push_back(std::to_string(i));
        }

        s.post([vec]{
                std::cout << "vec[middle]=" << vec.at(vec.size() / 2) << std::endl;
        });
    }
}
