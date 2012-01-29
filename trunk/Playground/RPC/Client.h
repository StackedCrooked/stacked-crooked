#ifndef CLIENT_H
#define CLIENT_H


#include "Serialization.h"
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <vector>


/**
 * Redirector redirects the RPC calls.
 *
 * Simply allocate a Redirector object on the stack and it will redirect calls
 * for the duration of its lifetime.
 *
 * You need to create at least one Redirector object in order to direct the
 * data to it's original location.
 *
 * For example to direct the output to an UDPClient object:
 *
 *  {
 *     UDPClient client("127.0.0.1", 9001);
 *     Redirector dest(boost::bind(&UDPClient::send, &client, _1));
 *     // ...
 *  }
 */
struct Redirector : boost::noncopyable
{
public:
    typedef boost::function<std::string(const std::string &)> Handler;

    static bool IsSet()
    {
        return !GetDestinations().empty();
    }

    static Redirector & Get()
    {
        return *GetDestinations().back();
    }

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

    Redirector(const Handler & inHandler) :
        mHandler(inHandler)
    {
        GetDestinations().push_back(this);
    }

    ~Redirector()
    {
        GetDestinations().pop_back();
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
