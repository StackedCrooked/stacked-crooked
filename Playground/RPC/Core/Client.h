#ifndef CLIENT_H
#define CLIENT_H


#include "Serialization.h"
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <vector>


/**
 * Redirector for directing or intercepting send data.
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

    static Redirector & Get()
    {
        if (Empty())
        {
            throw std::runtime_error("No Redirectors.");
        }
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

    const Handler & getHandler() const { return mHandler; }

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
