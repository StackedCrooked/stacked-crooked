#include <cassert>
#include <cstddef>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include <memory>
#include <mutex>


#define TRACE std::cout << __PRETTY_FUNCTION__ << std::endl;


template<class T, template<typename> class BeforeAndAfter>
class Proxy;


template<typename T>
struct Wrap;


template<typename T>
Wrap<T> wrap(const Proxy<T> &);


template<class T, template<typename> class BeforeAndAfter>
class Proxy : private BeforeAndAfter<T>
{
public:
    template<typename ...Args>
    explicit Proxy(Args && ...inArgs) :
        mObject(std::forward<Args>(inArgs)...)
    {
    }

    Wrap<T> operator->() const
    {
        return Wrap<T>(*this);
    }

private:
    friend class Wrap<T>;
    T mObject;
};


template<typename T>
struct Wrap
{
    Wrap(Proxy<T> & proxy)
    {
        proxy.before();
    }

    ~Wrap()
    {
        try
        {
            proxy.after();
        }
        catch (...)
        {
        }
    }
};


struct Item
{
    void foo() const { TRACE }
    void bar() const { TRACE }
};


namespace Hooking {

template<typename T>
struct Logger
{
    void before() { TRACE }
    void after() { TRACE }
};


template<typename T>
struct Locker
{
    void before() { mtx.lock(); }
    void after() { mtx.unlock(); }

private:
    Locker(Locker<T>&&) = default;
    Locker<T>& operator=(Locker<T>&&) = delete;

    Locker(const Locker<T>&) = delete;
    Locker<T>& operator=(const Locker<T>&) = delete;

    std::mutex mtx;
};

}


int main() {

    std::cout << "\nWith std::string:" << std::endl;
    Proxy<std::string, Locker> s;

    std::cout << "\nPushing back a character: " << std::endl;
    s->push_back('a');

    std::cout << "\nClearing the string: " << std::endl;
    s->clear();
}
