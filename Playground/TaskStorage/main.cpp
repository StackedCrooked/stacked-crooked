#include <iostream>
#include <array>
#include <string>
#include <vector>





struct Scheduler
{
    Scheduler()
    {
		mPool.reserve(mLocalStorages.size());
		for (auto& local_storage : mLocalStorages)
		{
			mPool.push_back(&local_storage);
		}
    }


    template<typename F>
    void post(F&& f)
    {
		mTasks.push_back(Create(std::forward<F>(f), mPool));

    }

    void run()
    {
        for (TaskImplBase* taskImplBase : mTasks)
        {
            taskImplBase->call();
            taskImplBase->dispose(mPool);
        }
		mTasks.clear();
    }

private:
    struct TaskImplBase;

	using LocalStorage = typename std::aligned_storage<32, 32>::type;
	using Pool = std::vector<LocalStorage*>;

    struct TaskImplBase
    {
        virtual ~TaskImplBase() { }
        virtual void call() = 0;
        virtual void dispose(Pool& pool) = 0;
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
			std::cout << "Calling this: " << typeid(*this).name() << std::endl;
            mF();
        }

        void dispose(Pool& pool) override final
        {
            if (HasLocalStorage)
            {
				std::cout << "In-place destruction of this: " << typeid(*this).name() << std::endl;
                this->~TaskImpl();
				//pool.push_back(this);
				(void)pool;
            }
            else
            {
				std::cout << "Deleting this: " << typeid(*this).name() << std::endl;
                delete this;
            }
        }

        F mF;
    };

	template<typename F>
	static auto Create(F f, Pool& pool) -> TaskImplBase*
	{
		if (sizeof(F) <= sizeof(LocalStorage))
		{
			auto result = new (pool.back()) TaskImpl<F, true>(std::move(f));
			pool.pop_back();
			return result;
		}
		else
		{
			return new TaskImpl<F, false>(std::move(f));
		}
	}

	std::array<LocalStorage, 1024> mLocalStorages;
	Pool mPool;
	std::vector<TaskImplBase*> mTasks;
};



int main()
{
    Scheduler s;
    s.post([]{});

    std::array<char, 1024> big_array;
    s.post([big_array]{});


	std::string abc("abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc");

	s.post([abc] {
		std::cout << abc << std::endl;
	});

	std::array<std::string, 4> strings;
	for (auto& s : strings)
	{
		s = abc;
	}

	s.post([strings]{
		for (auto& s : strings) 
		{
			std::cout << "s=" << s << std::endl;
		}
	});

	s.run();
}
