#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "RPC/Command.h"
#include "RPC/RemoteObjects.h"
#include <boost/tuple/tuple_io.hpp>


namespace RPC {


using boost::tuples::get;
typedef std::string URL;

template<typename T>
struct Identity { typedef T Type; };

template<typename Archive, typename Tuple>
void serialize_tuple(Archive & ar, Tuple & tuple)
{
    serialize_tuple_impl(ar, tuple, Identity<Tuple>());
}


template<typename Archive, typename Tuple>
void serialize_tuple_impl(Archive & ar, Tuple & tuple, const Identity<Tuple> &)
{
    ar & tuple.get_head();
    serialize_tuple_impl(ar, tuple.get_tail(), Identity<typename Tuple::tail_type>());
}

template<typename Archive, typename Tuple>
void serialize_tuple_impl(Archive &, const Tuple &, const Identity<boost::tuples::null_type> &)
{
}


struct CreateStopwatchArgs
{
public:
    CreateStopwatchArgs() {}

    CreateStopwatchArgs(const RemoteServer & inServer, const std::string & inName) :
        mTuple(inServer, inName)
    {
    }

    const RemoteServer & server() const { return mTuple.get<0>(); }

    const std::string & name() const { return mTuple.get<1>(); }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        serialize_tuple(ar, mTuple);
    }

    boost::tuples::tuple<RemoteServer, std::string> mTuple;
};


struct CreateStopwatch : public ConcreteCommand<RemoteStopwatch(CreateStopwatchArgs)>
{
    typedef ConcreteCommand<RemoteStopwatch(CreateStopwatchArgs)> Super;

    static const char * CommandName() { return "CreateStopwatch"; }

    CreateStopwatch(const RemoteServer & inServer, const std::string & inName) :
        Super(CommandName(), CreateStopwatchArgs(inServer, inName))
    {
    }
};


struct StartStopwatch : public ConcreteCommand<bool(RemoteStopwatch)>
{
    typedef ConcreteCommand<bool(RemoteStopwatch)> Super;

    static const char * CommandName() { return "StartStopwatch"; }

    StartStopwatch(const RemoteStopwatch & inStopwatch) :
        Super(CommandName(), inStopwatch)
    {
    }
};


} // namespace RPC


#endif // RPC_COMMANDS_H
