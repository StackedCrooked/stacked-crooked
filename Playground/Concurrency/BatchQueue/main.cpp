#include <tbb/concurrent_queue.h>
#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <assert.h>


void print(const std::string& c)
{
    static std::mutex m;
    std::unique_lock<std::mutex> lock(m);
    std::cout << c << ' ' << std::endl;
}


#define print(c) print(c);


template<typename T>
struct Batch
{
    enum { Capacity = 2 };
    
    Batch() : items(), size(), cap(Capacity) {}
    
    bool batch_push(T t)
    {
        items[size] = std::move(t);
        size++;
        cap = size;
        return size == Capacity;
    }

    bool consumer_empty() const { return size == 0; }
    
    
    bool pop(T& t)
    {
        if (size > 0) {
            t = std::move(items[cap - size]);
            size--;
            return true;
        }
        return false;
    }
    
    T items[Capacity];
    uint16_t size;
    uint16_t cap;
};


template<typename T>
struct ConcurrentBatchQueue
{
    ConcurrentBatchQueue(std::size_t capacity = 3) :
        mQueue()
    {
        mQueue.set_capacity(capacity);
    }

    ~ConcurrentBatchQueue()
    {
        if (!mProducer.consumer_empty())
        {
            std::abort();
        }
    }
    
    void push(T t)
    {
        if (mProducer.batch_push(t))
        {
            flush();
        }
    }

    void flush()
    {
        if (!mProducer.consumer_empty()) {
            mQueue.push(mProducer);
            mProducer = Batch<T>();
        } else {
            print("FAILEDFLUSH");
        }
    }
    
    void pop(T& t)
    {
        if (mConsumer.pop(t))
        {
            return;
        }

        mQueue.pop(mConsumer);
        if (!mConsumer.pop(t))
        {
            throw std::runtime_error("Empty Batch was popped.");
        }
    }

    Batch<T> mProducer;
    char padding1[128];
    tbb::concurrent_bounded_queue<Batch<T>> mQueue;
    char padding2[128];
    Batch<T> mConsumer;
    
};



int main()
{

    std::thread([]{ std::this_thread::sleep_for(std::chrono::seconds(1000)); std::cerr << "TIMEOUT" << std::endl; std::abort(); }).detach();

    ConcurrentBatchQueue<unsigned> q;

    unsigned count = 5;
        
    std::thread producer([&] {
        for (unsigned i = 1; i <= count; ++i) q.push(i);
        q.flush();
    });
    
    std::thread consumer([&]{
        for (;;)
        {
            unsigned n = 0;
            q.pop(n);
            print("pop " + std::to_string(n));
            if (n == count) return;
        }
    });
    producer.join();
    consumer.join();

    print("SUCCESS!");
}
