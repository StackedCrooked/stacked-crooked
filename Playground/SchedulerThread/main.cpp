#include "folly/ThreadCachedArena.h"
#include "folly/IndexedMemPool.h"
#include "folly/MPMCQueue.h"
#include <iostream>


struct TaskQueue
{
    TaskQueue()  :mQueue(1000)
    {
    }

    template<typename F>
    void post(const F& f)
    {
        auto index = mPool.allocIndex(f);
        mQueue.blockingWrite(index);
    }

    void poll_one()
    {
        std::uint32_t index;
        mQueue.blockingRead(index);
        mPool[index]
        task.run();
    }

    void poll_all()
    {
        auto n = mQueue.writeCount();
        std::cout << "writeCount=" << n << std::endl;

        while (n--)
        {
            TaskHolder task;
            mQueue.blockingRead(task);
            task.run();
        }

    }

private:
    struct TaskHolder
    {
        TaskHolder() :
            mCallback(),
            mTask()
        {
        }

        template<typename F>
        TaskHolder(F* f) :
            mCallback([](void* ptr) { (*static_cast<F*>(ptr))(); }),
            mTask(f)
        {
        }

        void run() { mCallback(mTask); }

        void (*mCallback)(void*);
        void* mTask;
    };


    static_assert(std::is_trivially_destructible<TaskHolder>::value, "");
    static_assert(std::is_trivially_copyable<TaskHolder>::value, "");


    folly::IndexedMemPool mPool;
    folly::MPMCQueue<uint32_t> mQueue;
};


int main()
{
    TaskQueue q;
    q.post([]{ std::cout << "Hello World!" << std::endl; });
    q.post([]{ std::cout << "Hello World!" << std::endl; });
    q.poll_one();
    q.post([]{ std::cout << "Hello World!" << std::endl; });
    q.poll_all();
}
