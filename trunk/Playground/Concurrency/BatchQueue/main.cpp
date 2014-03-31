#include <tbb/concurrent_queue.h>
#include <atomic>
#include <iostream>
#include <memory>
#include <thread>
#include <assert.h>


template<typename T>
struct Batch
{
    enum { Capacity = 10 };
    
    Batch() : items(), size() {}
    
    bool push(T t)
    {
        items[size++] = std::move(t);
        return size == Capacity;
    }
    
    
    bool pop(T& t)
    {
        if (size == 0)
        {
            std::cout << "Batch::pop: empty" << std::endl;
            return false;
        }
        auto index = Capacity - size;
        std::cout << "Batch::pop: size=" << size << " => index=" << index << std::endl;
        if (index == Capacity)
        {
            return false;
        }
        
        t = std::move(items[index]);
        size++;
        return true;
    }
    
    T items[Capacity];
    uint16_t size;
};


template<typename T>
struct ConcurrentBatchQueue
{
    ConcurrentBatchQueue(std::size_t capacity) :
        mQueue(),
        mBatchPool(new Batch<T>[capacity]),
        mIndex(),
        mPoppedBatch()
    {
        mQueue.set_capacity(capacity - 1);
    }
    
    void push(T t)
    {
        auto& batch = mBatchPool.get()[mIndex];
        if (batch.push(std::move(t)))
        {
            flush();
        }
    }
    
    void flush()
    {
        auto& current_batch = mBatchPool[mIndex];
        mQueue.push(&current_batch);
        current_batch = Batch<T>();
        mIndex = (mIndex + 1) % mQueue.capacity();
    }
    
    bool pop(T& t)
    {
		if (!mPoppedBatch)
		{
			mQueue.pop(mPoppedBatch);
            return mPoppedBatch->pop(t);
		}

        if (!mPoppedBatch->pop(t))
        {
            mQueue.pop(mPoppedBatch);
            return mPoppedBatch->pop(t);
        }

        return true;
    }
    
    tbb::concurrent_bounded_queue<Batch<T>*> mQueue;
    std::unique_ptr<Batch<T>[]> mBatchPool;
    std::size_t mIndex;
    
    char padding[128];
    
    Batch<T>* mPoppedBatch;
    
};


int main()
{

	std::thread([]{ std::this_thread::sleep_for(std::chrono::seconds(1)); std::cerr << "TIMEOUT" << std::endl; std::abort(); }).detach();
    ConcurrentBatchQueue<int> q(100);
    
    std::atomic<bool> quit(false);
        
    std::thread producer([&]{
        int n = 0;
        while (n < 200) {
            q.push(n);
            n++;
        }
    });
    
    std::thread consumer([&]{
        int n = 0;
        while (n != 200) {
            q.pop(n);
            std::cout << "Consumer popped: " << n << std::endl;
        }
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    quit = true;
    producer.join();
    consumer.join();
}
