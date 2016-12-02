#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <thread>
#include <iostream>





struct Worker
{
    Worker(boost::asio::io_service& ios) : ios(ios)
    {
        mTasks.reserve(100);
    }

    template<typename F>
    void post(F f)
    {
        std::cout << std::this_thread::get_id() << " PostedTask::post mTasks.size=" << mTasks.size() << std::endl;

        auto was_empty = mTasks.empty();
        mTasks.push_back(f);
        if (was_empty)
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
            std::cout << std::this_thread::get_id() << " PostedTask::call: worker_=" << worker_ << std::endl;
            if (!worker_)
            {
                throw std::runtime_error("std::bad");
            }
            worker_->run_tasks();
        }

        friend void* asio_handler_allocate(std::size_t size, PostedTask* task)
        {
            std::cout << std::this_thread::get_id() << " asio_handler_allocate size=" << size << " sizeof(*task)=" << sizeof(*task) << " sizeof(Storage)=" << sizeof(Storage) << std::endl;
            if (!task->worker_->mStorageInUse)
            {
                std::cout << std::this_thread::get_id() << " REUSE size=" << size << " sizeof(PostedTask)" << sizeof(PostedTask) << " &task->worker_->mStorage=" << (&task->worker_->mStorage) << std::endl;
                task->worker_->mStorageInUse = true;
                return &task->worker_->mStorage;
            }
            else
            {
                std::cout << std::this_thread::get_id() << " MALLOC size=" << size << " sizeof(PostedTask)" << sizeof(PostedTask) << " &task->worker_->mStorage=" << (&task->worker_->mStorage) << std::endl;
                return ::operator new (size);
            }
        }

        friend void asio_handler_deallocate(void* pointer, std::size_t size, PostedTask* task)
        {
            std::cout << std::this_thread::get_id() << " asio_handler_deallocate size=" << size << " sizeof(*task)=" << sizeof(*task) << " pointer=" << pointer << " task=" << task << std::endl;(void)size; (void)task;
            if (pointer == &task->worker_->mStorage)
            {
                task->worker_->mStorageInUse = false;
            }
            else
            {
                ::operator delete (pointer);
            }
        }

        Worker* worker_;
    };

    void run_tasks()
    {
        std::cout << std::this_thread::get_id() << " run_tasks: mTasks.size=" << mTasks.size() << std::endl;
        for (auto& task : mTasks)
        {
            task();
        }
        mTasks.clear();

    }

    using Function  = boost::function<void()>;

    using Storage = std::aligned_storage<sizeof(PostedTask), 32>::type;
    std::vector<Function> mTasks;
    bool mStorageInUse = false;
    Storage mStorage;
    boost::asio::io_service& ios;
};


    #define L(cmd) std::cout << __FILE__ << ":" << __LINE__ << ": " << std::flush << #cmd << std::endl; cmd

int main()
{
    std::cout << std::this_thread::get_id() << " Start of program" << std::endl;

    {
        boost::asio::io_service ios;

        {
            Worker worker(ios);
            ios.post([]{ std::cout << "I'm in a post!" << std::endl; });
            ios.run(); ios.reset();
            std::cout << "main.cpp:" << __LINE__ << std::endl;
            worker.post([]{ std::cout << std::this_thread::get_id() << " inside worker post! (post on main.cpp:" << __LINE__ << ")" << std::endl; });
            worker.post([]{ std::cout << std::this_thread::get_id() << " inside worker post! (post on main.cpp:" << __LINE__ << ")" << std::endl; });
            std::cout << "main.cpp:" << __LINE__ << std::endl;
            ios.run(); ios.reset();
            std::cout << "main.cpp:" << __LINE__ << std::endl;
            worker.post([]{ std::cout << std::this_thread::get_id() << " inside worker post! (post on main.cpp:" << __LINE__ << ")" << std::endl; });
            worker.post([]{ std::cout << std::this_thread::get_id() << " inside worker post! (post on main.cpp:" << __LINE__ << ")" << std::endl; });
            worker.post([]{ std::cout << std::this_thread::get_id() << " inside worker post! (post on main.cpp:" << __LINE__ << ")" << std::endl; });
            worker.post([]{ std::cout << std::this_thread::get_id() << " inside worker post! (post on main.cpp:" << __LINE__ << ")" << std::endl; });
            worker.post([]{ std::cout << std::this_thread::get_id() << " inside worker post! (post on main.cpp:" << __LINE__ << ")" << std::endl; });
            worker.post([]{ std::cout << std::this_thread::get_id() << " inside worker post! (post on main.cpp:" << __LINE__ << ")" << std::endl; });
            std::cout << "main.cpp:" << __LINE__ << std::endl;
            worker.post([]{ std::cout << std::this_thread::get_id() << " inside worker post! (post on main.cpp:" << __LINE__ << ")" << std::endl; });
            std::cout << "main.cpp:" << __LINE__ << std::endl;
            worker.ios.run();
            std::cout << "main.cpp:" << __LINE__ << std::endl;
        }
        std::cout << "main.cpp:" << __LINE__ << std::endl;
    }
    std::cout << "main.cpp:" << __LINE__ << std::endl;

}
