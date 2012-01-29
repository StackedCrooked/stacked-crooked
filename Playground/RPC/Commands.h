#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "Command.h"
#include "RemoteObjects.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <map>
#include <set>
#include <vector>


using boost::tuples::tuple;
typedef std::string Name;


struct Void
{
    template<class Archive>
    void serialize(Archive & , const unsigned int)
    {
    }
};


typedef boost::function<std::string(const std::string)> Runner;
typedef std::map<std::string, Runner> Runners;


inline Runners & GetRunners()
{
    static Runners fRunners;
    return fRunners;
}


template<typename Command>
inline void Register()
{
    GetRunners().insert(std::make_pair(Command::Name(), boost::bind(&Command::Run, _1)));
}


#define RPC_DECLARE_CALL(RET, NAME, ARG) \
    struct NAME : public ConcreteCommand<RET(ARG)> { \
        static const char * Name() { return #NAME; } \
        NAME(const Arg & inArgs) : Super(Name(), inArgs) { } \
        static std::string Run(const std::string & arg); \
        static RET Implement(const ARG & arg); \
    };

#if TARGET_IS_RPC_SERVER
#define RPC_IMPLEMENT_CALL(RET, NAME, ARG) \
    std::string NAME::Run(const std::string & arg_str) { \
        ARG arg = deserialize<ARG>(arg_str); \
        RET ret = NAME::Implement(arg); \
        return serialize(ret); \
    } \
    struct Register##NAME { Register##NAME() { Register<NAME>(); } } gRegister##NAME;
#endif


#if TARGET_IS_RPC_SERVER
#define RPC_CALL(RET, NAME, ARG) \
    RPC_DECLARE_CALL(RET, NAME, ARG) \
    RPC_IMPLEMENT_CALL(RET, NAME, ARG)
#else
#define RPC_CALL(RET, NAME, ARG) \
    RPC_DECLARE_CALL(RET, NAME, ARG)
#endif


RPC_CALL(RemoteStopwatch, Stopwatch_Create  , std::string     )
RPC_CALL(Void,            Stopwatch_Start   , RemoteStopwatch )
RPC_CALL(unsigned,        Stopwatch_Elapsed , RemoteStopwatch )
RPC_CALL(unsigned,        Stopwatch_Stop    , RemoteStopwatch )
RPC_CALL(Void,            Stopwatch_Destroy , RemoteStopwatch )


template<class C1, class C2,
         typename C1Arg = typename C1::Arg,
         typename C1Ret = typename C1::Ret,
         typename C2Arg = typename C2::Arg,
         typename C2Ret = typename C2::Ret>
struct ChainedCommand : public ConcreteCommand<C2Ret(C1Arg)>
{
    BOOST_STATIC_ASSERT_MSG((boost::is_same<C1Ret, C2Arg>::value), "Types don't line up correctly.");

    typedef ConcreteCommand<C2Ret(C1Arg)> Super;
    typedef typename Super::Arg Arg;
    typedef typename Super::Ret Ret;

    static const char * Name() { return "ChainedCommand"; }
    ChainedCommand(const Arg & inArg) : Super(Name(), inArg) { }
};


template< class C,
          typename Results = std::vector<typename C::Ret> ,
          typename Args    = std::vector<typename C::Arg> >
struct ParallelCommand : public ConcreteCommand<Results(Args)>
{
    typedef ConcreteCommand<std::vector<typename C::Ret>(std::vector<typename C::Arg>)> Super;
    typedef typename Super::Arg Arg;
    typedef typename Super::Ret Ret;

    static const char * Name() { return "ParallelCommand"; }
    ParallelCommand(const Arg & inArg) : Super(Name(), inArg) { }
};


//struct Registrator
//{
//    Registrator & Get()
//    {
//        static Registrator fInstance;
//        return fInstance;
//    }

//    template<typename Command>
//    void registerCommand()
//    {
//        mCommands.insert(Command::Name());
//    }

//    template<typename Command,
//             typename Ret     = typename Command::Ret
//             typename Arg     = typename Command::Arg,
//             typename Handler = boost::function<Ret(Arg)> >
//    void registerHandler(const Handler & inHandler)
//    {
//        std::string name = Command::Name();
//        if (mCommands.find(name) == mCommands.end())
//        {
//            throw std::runtime_error("Command is not yet registered.");
//        }
//        mHandler.insert(std::make_pair(Command::Name(), inHandler));
//    }

//    std::set<std::string> mCommands;
//    std::map<std::string, Handler> mHandler;
//};


#endif // RPC_COMMANDS_H
