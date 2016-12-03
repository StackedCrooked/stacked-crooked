#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/container/static_vector.hpp>
#include <atomic>
#include <thread>
#include <stdint.h>
#include <stdio.h>


#define trace() printf("... thread_id=0x%lx main.cpp:%d: %s\n", reinterpret_cast<intptr_t>(pthread_self()), __LINE__, __FUNCTION__)
#define say_trace(s) printf(">>> thread_id=0x%lx main.cpp:%d: %s MESSAGE=%s\n", reinterpret_cast<intptr_t>(pthread_self()), __LINE__, __FUNCTION__, s)
#define nice_trace() printf("+++ thread_id=0x%lx main.cpp:%d: %s\n", reinterpret_cast<intptr_t>(pthread_self()), __LINE__, __FUNCTION__)
#define super_trace() printf("*** thread_id=0x%lx main.cpp:%d: %s\n", reinterpret_cast<intptr_t>(pthread_self()), __LINE__, __FUNCTION__)

std::atomic<bool> enable_trace{true};

#define ohno_trace() if (enable_trace) []{printf("DEL thread_id=0x%lx main.cpp:%d:%s\n", reinterpret_cast<intptr_t>(pthread_self()), __LINE__, __FUNCTION__); }()
#define newo_trace(n) if (enable_trace) [n] { printf("NEW thread_id=0x%lx main.cpp:%d:%s: size=%d\n", reinterpret_cast<intptr_t>(pthread_self()), __LINE__, __FUNCTION__, int(n)); }()


struct Worker
{
    Worker(boost::asio::io_service& ios) : ios(ios)
    {
    }

    template<typename F>
    void post(F f)
    {
        trace();

        const auto old_size = mTasks.size();

        if (old_size < mTasks.capacity())
        {
            mTasks.push_back(f);
        }
        else
        {
            say_trace("mAdditionalTasks push!");
            mAdditionalTasks.push_back(f);
        }

        if (old_size == 0)
        {
            super_trace();
            ios.post(PostedTask(*this));
        }
    }

    struct PostedTask
    {
        PostedTask() : worker_() {}
        PostedTask(Worker& worker) : worker_(&worker) {}

        void operator()() const
        {
            super_trace();
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
                super_trace();
                task->worker_->mStorageInUse = true;
                return &task->worker_->mStorage;
            }
            else
            {
                ohno_trace();
                return ::operator new (size);
            }
        }

        friend void asio_handler_deallocate(void* pointer, std::size_t /*size*/, PostedTask* task)
        {
            if (pointer == &task->worker_->mStorage)
            {
                super_trace();
                task->worker_->mStorageInUse = false;
            }
            else
            {
                ohno_trace();
                ::operator delete (pointer);
            }
        }

        Worker* worker_;
    };

    void run_tasks()
    {
        super_trace();

        for (auto& task : mTasks)
        {
            task();
        }

        mTasks.clear();


        for (auto& task : mAdditionalTasks)
        {
            say_trace("mAdditionalTasks invocation!");
            task();
        }

        mAdditionalTasks.clear();
    }

    using Function  = boost::function<void()>;
    using Storage = std::aligned_storage<sizeof(PostedTask), 32>::type;

    boost::container::static_vector<Function, 2> mTasks;
    std::vector<Function> mAdditionalTasks;
    Storage mStorage;
    bool mStorageInUse = false;
    boost::asio::io_service& ios;
};




void run(boost::asio::io_service& ios)
{
    printf("<run>\n");
    trace();
    {
        trace();
        {
            printf("<section n=2>\n");
            trace();
            Worker worker(ios);
            trace();
            worker.post([]{ nice_trace(); });
            trace();
            worker.post([]{ nice_trace(); });
            trace();
            ios.run();
            trace();
            ios.reset();
            printf("</section>\n");
        }
        {
            printf("<section n=3>\n");
            trace();
            Worker worker(ios);
            trace();
            worker.post([]{ nice_trace(); });
            trace();
            worker.post([]{ nice_trace(); });
            trace();
            worker.post([]{ nice_trace(); });
            trace();
            worker.post([]{ nice_trace(); });
            trace();
            worker.post([]{ nice_trace(); });
            trace();
            worker.post([]{ nice_trace(); });
            trace();
            ios.run();
            trace();
            ios.reset();
            printf("</section>\n");
        }
        trace();
    }
    trace();
    printf("</run>\n");

}


int main()
{
    printf("<main>\n");
    {
        printf("<io_service>\n");
        boost::asio::io_service ios;
        run(ios);
        printf("</io_service>\n");
    }
    printf("</main>\n");
}



void * operator new(std::size_t n) throw(std::bad_alloc)
{
    newo_trace(n);
    return malloc(n);
}



void operator delete(void* p) throw()
{
    ohno_trace();
    free(p);
}


void *operator new[](std::size_t s) throw(std::bad_alloc)
{
    newo_trace(s);
    return malloc(s);
}


void operator delete[](void *p) throw()
{
    ohno_trace();
    free(p);
}
