#include <boost/container/static_vector.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <atomic>
#include <thread>
#include <stdint.h>
#include <stdio.h>


std::atomic<bool> enable_trace{true};


struct Worker
{
    Worker(boost::asio::io_service& ios) : ios(ios)
    {
    }

    template<typename F>
    void post(F f)
    {
        const auto old_size = mTasks.size();

        if (old_size < mTasks.capacity())
        {
            mTasks.push_back(f);
        }
        else
        {
            mAdditionalTasks.push_back(f);
        }

        if (old_size == 0)
        {
            ios.post(HelperTask(*this));
        }
    }

    struct HelperTask
    {
        HelperTask(Worker& worker) :
            mWorker(&worker)
        {
        }

        void operator()() const
        {
            mWorker->run_tasks();
        }

        friend void* asio_handler_allocate(std::size_t size, HelperTask* task)
        {
            if (!task->mWorker->mStorageInUse)
            {
                task->mWorker->mStorageInUse = true;
                return &task->mWorker->mStorage;
            }
            else
            {
                printf("    UNEXPECTED NEW size=%d\n", int(size));
                return ::operator new (size);
            }
        }

        friend void asio_handler_deallocate(void* pointer, std::size_t /*size*/, HelperTask* task)
        {
            if (pointer == &task->mWorker->mStorage)
            {
                task->mWorker->mStorageInUse = false;
            }
            else
            {
                printf("    UNEXPECTED DELETE\n");
                ::operator delete (pointer);
            }
        }
        Worker* mWorker;
    };

    void run_tasks()
    {
        for (auto& task : mTasks)
        {
            task();
        }

        for (auto& task : mAdditionalTasks)
        {
            task();
        }

        mTasks.clear();
        mAdditionalTasks.clear();
    }

    using Function  = boost::function<void()>;
    using Storage = std::aligned_storage<sizeof(HelperTask), 32>::type;

    boost::container::static_vector<Function, 4> mTasks;
    std::vector<Function> mAdditionalTasks; // used as a fallback
    Storage mStorage;
    bool mStorageInUse = false;
    boost::asio::io_service& ios;
};


int main()
{
    {
        printf("--- creating io_service object ---\n");
        boost::asio::io_service ios;

        Worker worker(ios);
        {
            printf("---- schedule few tasks ----\n");
            {
                for (auto i = 0; i != 4; ++i)
                {
                    printf("Posting task %d.\n", int(i + 1));
                    worker.post([i]{ printf("Running task %d!\n", int(i + 1)); });
                }
                ios.run();
                ios.reset();
            }
        }

        {
            printf("---- schedule more tasks ----\n");
            {
                for (auto i = 0; i != 8; ++i)
                {
                    printf("Posting task %d.\n", int(i + 1));
                    worker.post([i]{ printf("Running task %d!\n", int(i + 1)); });
                }
                ios.run();
                ios.reset();
            }
        }

        printf("---- destroying io_service object ----\n");
    }

    printf("---- end-of-main ----\n");
}


void * operator new(std::size_t n) throw(std::bad_alloc)
{
    if (enable_trace)
    {
        printf("main.cpp:%d: ::operator new(std::size_t n): n=%d\n", __LINE__, int(n));
    }
    return malloc(n);
}



void operator delete(void* p) throw()
{
    if (enable_trace)
    {
        printf("main.cpp:%d: ::operator delete(void* p): p=0x%lx\n", __LINE__, reinterpret_cast<std::intptr_t>(p));
    }
    free(p);
}


void *operator new[](std::size_t n) throw(std::bad_alloc)
{
    if (enable_trace)
    {
        printf("main.cpp:%d: ::operator new[](std::size_t n): n=%d\n", __LINE__, int(n));
    }
    return malloc(n);
}


void operator delete[](void *p) throw()
{
    if (enable_trace)
    {
        printf("main.cpp:%d: ::operator delete[](void* p): p=0x%lx\n", __LINE__, reinterpret_cast<std::intptr_t>(p));
    }
    free(p);
}
