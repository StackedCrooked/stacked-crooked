#include "tbb/concurrent_queue.h"
#include <atomic>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>

#define TRACE std::cout << __PRETTY_FUNCTION__ << std::endl;

struct Stack;
struct Protocol;
struct Session;

class Internal {
    Internal() {} // constructor is private
    friend struct Stack; // Only the Stack can create it.
};

struct Session
{
    Session(Protocol& protocol);

    // disable copy and copy assignment
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    // enable move and move assignment
    Session(Session&&) = default;
    Session& operator=(Session&&) = default;

    bool start(Internal) {
        TRACE
        return true;
    }

    bool stop(Internal) {
        TRACE
        return true;
    }

    std::future<bool> start();
    std::future<bool> stop();

private:
    Protocol* protocol_;
    Stack* stack_;
};

struct Protocol
{
    Protocol(Stack& stack);

    // disable copy and copy assignment
    Protocol(const Protocol&) = delete;
    Protocol& operator=(const Protocol&) = delete;

    // enable move and move assignment
    Protocol(Protocol&&) = default;
    Protocol& operator=(Protocol&&) = default;

    Stack& getStack();

    Session& addSession(Internal) {
        TRACE
        mSessions.emplace_back(*this);
        return mSessions.back();
    }

    bool stop_all(Internal internal) {
        TRACE
        for (auto& session : mSessions)
            session.stop(internal);
        return true;
    }

    std::future<Session*> addSession();
    std::future<bool> stop_all();

private:

    Stack* stack_;
    std::vector<Session> mSessions;
};

struct Stack
{
    Stack() :
        mQuit(false),
        mInternalThread([=] { this->internal_thread(); })
    {
        TRACE
    }

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    ~Stack()
    {
        stop_all();
        mInternalThread.join();
    }

    bool stop_all(Internal internal) {
        TRACE
        mQuit = true;
        for (auto& protocol : mProtocols)
            protocol.stop_all(internal);
        return true;
    }

    std::future<bool> stop_all() {
        TRACE
        mQuit = true; // atomic may be set from main thread
        return schedule([=](Internal internal){
            return stop_all(internal);
        });
    }

    Protocol& addProtocol(Internal) {
        TRACE
        mProtocols.emplace_back(*this);
        return mProtocols.back();
    }

    std::future<Protocol*> addProtocol() {
        TRACE
        return schedule([=](Internal internal) {
            return &addProtocol(internal);
        });
    }


    template<typename F>
    auto schedule(F&& f) -> std::future<decltype(f(std::declval<Internal>()))>
    {
        auto promise = std::make_shared<std::promise<decltype(f(std::declval<Internal>()))>>();
        mTasks.push([=] {
            promise->set_value(f(Internal{}));
        });
        return promise->get_future();
    }

private:
    void internal_thread() {
        Task task;
        for (;;) {
            if (mQuit) return;
            while (mTasks.try_pop(task)) {
                if (mQuit) return;
                task();
            }

            if (mQuit) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    std::atomic<bool> mQuit;
    std::vector<Protocol> mProtocols;

    typedef std::function<void()> Task;
    tbb::concurrent_bounded_queue<Task> mTasks;
    std::thread mInternalThread;
};


Protocol::Protocol(Stack& stack) : stack_(&stack) {
    TRACE
}

Stack& Protocol::getStack() { return *stack_; }

std::future<Session*> Protocol::addSession() {
    TRACE
    return stack_->schedule([=](Internal internal){
        return &addSession(internal);
    });
}

std::future<bool> Protocol::stop_all() {
    TRACE
    return stack_->schedule([=](Internal internal){
        return stop_all(internal);
    });
}

Session::Session(Protocol& protocol) : protocol_(&protocol), stack_(&protocol.getStack()) {
    TRACE
}

std::future<bool> Session::start() {
    TRACE
    return stack_->schedule([=](Internal internal) {
        return this->start(internal);
    });
}

std::future<bool> Session::stop() {
    TRACE
    return stack_->schedule([=](Internal internal) {
        return this->stop(internal);
    });
}


int main()
{
    Stack stack;

    Protocol& protocol = *stack.schedule([&](Internal internal) {
        return &stack.addProtocol(internal);
    }).get();

    Session& session = *stack.schedule([&](Internal internal) {
        return &protocol.addSession(internal);
    }).get();

    stack.schedule([&](Internal internal) {
        return session.start(internal);
    }).get();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    stack.stop_all().wait_for(std::chrono::seconds(1));
}
