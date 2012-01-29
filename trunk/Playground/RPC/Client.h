#ifndef CLIENT_H
#define CLIENT_H


#include "Serialization.h"
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <vector>


/**
 * Redirector (re-)directs the calls to `RemoteCall::send()`, for example
 * when calling: `DoSomething(obj).send();`
 *
 * Allocate on the stack and provide a function object to indicate where the
 * data should go to. The redirection will remain for the duration of the
 * object's lifetime. Once the redirector goes out of scope the previous one
 * becomes active.
 *
 * There needs to be at least one Redirector object or else the data will have
 * no destination. An exception will be thrown if you try to send data without
 * having a redirector.
 *
 * Usage example:
 *
 *     // Create an UDP client object.
 *     UDPClient client("127.0.0.1", 9001);
 *
 *     // All
 *     Redirector dest(boost::bind(&UDPClient::send, &client, _1));
 *     // ...
 *  }
 */
struct Redirector : boost::noncopyable
{
public:
    typedef boost::function<std::string(const std::string &)> Handler;

    Redirector(const Handler & inHandler) :
        mHandler(inHandler)
    {
        GetDestinations().push_back(this);
    }

    ~Redirector()
    {
        GetDestinations().pop_back();
    }

    static bool Empty() { return GetDestinations().empty(); }

    static Redirector & Get() { return *GetDestinations().back(); }

    template<typename Command>
    typename Command::Ret send(const Command & command)
    {
        std::string result = mHandler(serialize(NameAndArg(command.name(), serialize(command.arg()))));
        RetOrError retOrError = deserialize<RetOrError>(result);
        if (retOrError.get_head())
        {
            return deserialize<typename Command::Ret>(retOrError.get<1>());
        }
        else
        {
            throw std::runtime_error("Server error: " + retOrError.get<1>());
        }
    }

private:
    typedef std::vector<Redirector*> Destinations;
    static Destinations & GetDestinations()
    {
        static Destinations fDestinations;
        return fDestinations;
    }

    Handler mHandler;
};



#endif // CLIENT_H
