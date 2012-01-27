#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "RPC/Command.h"
#include "RPC/RemoteObjects.h"
#include <boost/tuple/tuple_io.hpp>


namespace RPC {


using boost::tuples::get;
using boost::tuples::tuple;
typedef std::string URL;


template<class Tuple>
struct Helper
{
    template<typename Archive>
    static void serialize(Archive & ar, Tuple & tuple)
    {
        ar & tuple.get_head();
        Helper<typename Tuple::tail_type>::serialize(ar, tuple.get_tail());
    }
};


template<>
struct Helper<boost::tuples::null_type>
{
    template<typename Archive>
    static void serialize(Archive &, const boost::tuples::null_type &) { }
};


template<typename Archive, typename Tuple>
void serialize_tuple(Archive & ar, Tuple & tuple)
{

    Helper<Tuple>::serialize(ar, tuple);
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

    tuple<RemoteServer, std::string> mTuple;
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
