#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "Command.h"
#include "Serialization.h"


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
RPC_CALL(Void,            Stopwatch_Restart , RemoteStopwatch )
RPC_CALL(Void,            Stopwatch_Destroy , RemoteStopwatch )


#endif // RPC_COMMANDS_H
