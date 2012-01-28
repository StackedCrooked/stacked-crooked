#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "RPC/Command.h"
#include "RPC/RemoteObjects.h"
#include <vector>


namespace RPC {


using boost::tuples::tuple;
typedef std::string Name;


struct CreateStopwatch : public ConcreteCommand<RemoteStopwatch(tuple<RemoteServer, Name>)>
{
    static const char * CommandName() { return "CreateStopwatch"; }
    CreateStopwatch(const Arg & inArgs) : Super(CommandName(), inArgs) { }
};


struct StartStopwatch : public ConcreteCommand<bool(RemoteStopwatch)>
{
    static const char * CommandName() { return "StartStopwatch"; }
    StartStopwatch(const Arg & inArg) : Super(CommandName(), inArg) { }
};


struct StopStopwatch : public ConcreteCommand<unsigned(RemoteStopwatch)>
{
    static const char * CommandName() { return "StopStopwatch"; }
    StopStopwatch(const Arg & inArg) : Super(CommandName(), inArg) { }
};


template<class C0, class C1>
struct ChainedCommand : public ConcreteCommand<typename C1::Ret(typename C0::Arg)>
{
    typedef ConcreteCommand<typename C1::Ret(typename C0::Arg)> Super;
    typedef typename Super::Arg Arg;
    typedef typename Super::Ret Ret;

    static const char * CommandName() { return "ChainedCommand"; }
    ChainedCommand(const Arg & inArg) : Super(CommandName(), inArg) { }
};


template< class C,
          typename Results = std::vector<typename C::Ret> ,
          typename Args    = std::vector<typename C::Arg> >
struct ParallelCommand : public ConcreteCommand<Results(Args)>
{
    typedef ConcreteCommand<std::vector<typename C::Ret>(std::vector<typename C::Arg>)> Super;
    typedef typename Super::Arg Arg;
    typedef typename Super::Ret Ret;

    static const char * CommandName() { return "ParallelCommand"; }
    ParallelCommand(const Arg & inArg) : Super(CommandName(), inArg) { }
};



} // namespace RPC


#endif // RPC_COMMANDS_H
