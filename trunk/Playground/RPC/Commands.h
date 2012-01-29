#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "Command.h"


#if TARGET_IS_RPC_SERVER
#define RPC_DECLARE_CALL(RET, NAME, ARG) \
    struct NAME : public ConcreteCommand<RET(ARG)> { \
        static std::string Name() { return #NAME; } \
        NAME(const Arg & inArgs) : ConcreteCommand<RET(ARG)>(Name(), inArgs) { } \
        static std::string Run(const std::string & arg); \
        static RET Implement(const ARG & arg); \
    };

#define RPC_IMPLEMENT_CALL(RET, NAME, ARG) \
    std::string NAME::Run(const std::string & arg_str) { \
        return serialize(NAME::Implement(deserialize<ARG>(arg_str))); \
    } \
    struct Register##NAME { Register##NAME() { Register<NAME>(); } } gRegister##NAME;

#define RPC_CALL(R, N, A) \
    RPC_DECLARE_CALL(R, N, A) \
    RPC_IMPLEMENT_CALL(R, N, A)

#else // TARGET_IS_RPC_SERVER

#define RPC_CALL(RET, NAME, ARG) \
    struct NAME : public ConcreteCommand<RET(ARG)> { \
        static std::string Name() { return #NAME; } \
        NAME(const Arg & inArgs) : ConcreteCommand<RET(ARG)>(Name(), inArgs) { } \
    };

#endif // TARGET_IS_RPC_SERVER


//
// RPC_CALL: quickly define a new RPC call.
// Order of parameters: Return type, name, arg type
//
RPC_CALL(RemoteStopwatch, CreateServer     , std::string     )
RPC_CALL(Void,            StartStopwatch   , RemoteStopwatch )
RPC_CALL(unsigned,        CheckStopwatch   , RemoteStopwatch )
RPC_CALL(unsigned,        StopStopwatch    , RemoteStopwatch )
RPC_CALL(Void,            DestroyStopwatch , RemoteStopwatch )


#endif // RPC_COMMANDS_H
