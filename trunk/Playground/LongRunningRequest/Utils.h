#ifndef UTILS_H
#define UTILS_H


#include <cstddef>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <boost/preprocessor/cat.hpp>


template<typename C>
struct notifier_t
{
    notifier_t(C & c) : c(c) {}

    void operator()() const
    {
        c.notify_one();
    }

    C & c;

};


template<class T>
class ConcurrentObject
{
public:
    void notify() { cond.notify_one(); }

    T copy() const
    {
        return obj;
    }

    const T * operator -> () const
    {
        return &obj;
    }

    T * operator -> ()
    {
        std::unique_ptr<void, decltype(&_notify)> ptr(nullptr, &_notify);
        return &obj;
    }

private:
    static void _notify(std::condition_variable & c) { c.notify_one(); }

    mutable std::condition_variable cond;
    mutable std::mutex mtx;
    T obj;
};



struct A
{
    A()
    {
        ConcurrentObject<std::string> c;
        c.copy();
        std::cout << "APPEND" << std::endl;
        c->append('a');
    }

};
A a;


#define LOCK(obj) std::unique_lock<std::mutex> BOOST_PP_CAT(lock, __COUNTER__)(obj.mtx)


template<typename T>
class ConcurrentQueue
{
public:
    void push(T && t)
    {
        LOCK(queue);
        queue->emplace_back(t);
    }

    T pop()
    {
        LOCK(queue);
        auto result = queue->front();
        queue->pop_front();
        return result;
        if (queue->empty())
        {

        }
        return queue.front();
    }

private:
    typedef std::deque<T> queue_t;
    ConcurrentObject<queue_t> queue;
};


#endif // UTILS_H
