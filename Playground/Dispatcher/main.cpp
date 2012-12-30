#include <iostream>
#include <functional>
#include <string>
#include <boost/thread/condition.hpp>
#include <boost/thread/future.hpp>
#include <boost/thread/mutex.hpp>


typedef boost::mutex Mutex;
typedef boost::unique_lock<Mutex> Lock;
typedef boost::condition_variable Condition;


struct Dispatcher
{
    Dispatcher() :
        mUserServer(9000, std::bind(&Dispatcher::handleClientRequest, this, std::placeholders::_1)),
        mJobServer(9001, std::bind(&Dispatcher::handleJobRequest, this, std::placeholders::_1)),
        mResultReceiver(9002, std::bind(&Dispatcher::receiveResult, this, std::placeholders::_1))
    {
    }

    std::string handleClientRequest(const std::string & inRequest)
    {
        {
            Lock lock(mJobMutex);
            mJob = inRequest;
            mJobCondition.notify_one();
        }

        {
            Lock lock(mResultMutex);
            mResultCondition.wait(lock);
            return mResult;
        }
    }

    std::string handleJobRequest(const std::string &)
    {
        Lock l(mJobMutex);
        mJobCondition.wait(l);
        return mJob;
    }

    bool receiveResult(const std::string & str)
    {
        Lock lock(mResultMutex);
        mResult = str;
        mResultCondition.notify_one();
        return false;
    }

    UDPServer mUserServer;
    UDPServer mJobServer;
    UDPReceiver mResultReceiver;

    Condition mJobCondition;
    Mutex mJobMutex;
    std::string mJob;

    Condition mResultCondition;
    Mutex mResultMutex;
    std::string mResult;
};


int main()
{
    Dispatcher broker;
}
