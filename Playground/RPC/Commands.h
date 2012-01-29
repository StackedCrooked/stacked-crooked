#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "Command.h"


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
        return serialize(NAME::Implement(deserialize<ARG>(arg_str))); \
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


#endif // RPC_COMMANDS_H
