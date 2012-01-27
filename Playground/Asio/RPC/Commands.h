#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "RPC/Command.h"
#include "RPC/RemoteObjects.h"


namespace boost {
namespace serialization {


using boost::tuples::tuple;


template<class Archive, class T0>
void serialize(Archive & ar, tuple<T0> & value, const unsigned int)
{
    RPC::serialize_tuple(ar, value);
}


template<class Archive, typename T0, typename T1>
void serialize(Archive & ar, tuple<T0, T1> & value, const unsigned int)
{
    RPC::serialize_tuple(ar, value);
}


template<class Archive, typename T0, typename T1, typename T2>
void serialize(Archive & ar, tuple<T0, T1, T2> & value, const unsigned int)
{
    RPC::serialize_tuple(ar, value);
}


} // namespace serialization
} // namespace boost


namespace RPC {


using boost::tuples::tuple;
typedef std::string Name;


struct CreateStopwatch : public ConcreteCommand<RemoteStopwatch(tuple<RemoteServer, Name>)>
{
    static const char * CommandName() { return "CreateStopwatch"; }

    CreateStopwatch(const Arg & inArgs) :
        Super(CommandName(), inArgs)
    {
    }
};


struct StartStopwatch : public ConcreteCommand<bool(RemoteStopwatch)>
{
    static const char * CommandName() { return "StartStopwatch"; }

    StartStopwatch(const RemoteStopwatch & inStopwatch) :
        Super(CommandName(), inStopwatch)
    {
    }
};


} // namespace RPC


#endif // RPC_COMMANDS_H
