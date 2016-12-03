#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/container/static_vector.hpp>
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
            ios.post(PostedTask(*this));
        }
    }

    struct PostedTask
    {
        PostedTask() : worker_() {}
        PostedTask(Worker& worker) : worker_(&worker) {}

        void operator()() const
        {
            if (!worker_)
            {
                throw std::runtime_error("std::bad");
            }
            worker_->run_tasks();
        }

        friend void* asio_handler_allocate(std::size_t size, PostedTask* task)
        {
            if (!task->worker_->mStorageInUse)
            {
                task->worker_->mStorageInUse = true;
                return &task->worker_->mStorage;
            }
            else
            {
                printf("    NEW size=%d\n", int(size));
                return ::operator new (size);
            }
        }

        friend void asio_handler_deallocate(void* pointer, std::size_t /*size*/, PostedTask* task)
        {
            if (pointer == &task->worker_->mStorage)
            {
                task->worker_->mStorageInUse = false;
            }
            else
            {
                printf("    DEL\n");
                ::operator delete (pointer);
            }
        }

        Worker* worker_;
    };

    void run_tasks()
    {

        for (auto& task : mTasks)
        {
            task();
        }

        mTasks.clear();


        for (auto& task : mAdditionalTasks)
        {
            task();
        }

        mAdditionalTasks.clear();
    }

    using Function  = boost::function<void()>;
    using Storage = std::aligned_storage<sizeof(PostedTask), 32>::type;

    boost::container::static_vector<Function, 4> mTasks;
    std::vector<Function> mAdditionalTasks;
    Storage mStorage;
    bool mStorageInUse = false;
    boost::asio::io_service& ios;
};




void run(boost::asio::io_service& ios)
{
    {
        printf("---- few tasks (no allocs) ----\n");
        {
            Worker worker(ios);
            for (auto i = 0; i != 4; ++i)
            {
                worker.post([i]{ printf("Invocation of task %d\n", int(i + 1)); });
            }
            ios.run();
            ios.reset();
        }
        printf("---- end of few tasks ----\n");
    }
    {
        printf("\n---- many asks ----\n");
        {
            Worker worker(ios);
            for (auto i = 0; i != 8; ++i)
            {
                worker.post([i]{ printf("Invocation of task %d\n", int(i + 1)); });
            }
            ios.run();
            ios.reset();
        }
        printf("---- end of many tasks ----\n");
    }
}


int main()
{
    printf("============ start of main ============\n");
    boost::asio::io_service ios;
    run(ios);
    printf("============ end of main ============\n");
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
