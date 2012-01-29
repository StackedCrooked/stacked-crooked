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
        TRACE \
        return serialize(NAME::Implement(deserialize<ARG>(arg_str))); \
    } \
    struct Register##NAME { Register##NAME() { Register<NAME>(); } } gRegister##NAME;

#define RPC_BATCH_CALL(NAME, ARG) \
    struct NAME##Batch : public BatchCommand<NAME> { \
        NAME##Batch(const std::vector<ARG> & args) : \
            BatchCommand<NAME>(args) { } \
    }; \
    struct NAME##Batch##Registrator { \
        NAME##Batch##Registrator() { \
            Register<NAME##Batch>(); \
        } \
    } g##NAME##Batch##Registrator;

#define RPC_CALL(R, N, A) \
    RPC_DECLARE_CALL(R, N, A) \
    RPC_IMPLEMENT_CALL(R, N, A) \
    RPC_BATCH_CALL(N, A)

#else

#define RPC_BATCH_CALL(NAME, ARG) \
    struct NAME##Batch : public BatchCommand<NAME> { \
        NAME##Batch(const std::vector<ARG> & args) : \
            BatchCommand<NAME>(args) { } \
    };

#define RPC_CALL(RET, NAME, ARG) \
    struct NAME : public ConcreteCommand<RET(ARG)> { \
        static std::string Name() { return #NAME; } \
        NAME(const Arg & inArgs) : ConcreteCommand<RET(ARG)>(Name(), inArgs) { } \
    }; \
    RPC_BATCH_CALL(NAME, ARG)
#endif // TARGET_IS_RPC_SERVER


//
// RPC_CALL: quickly define a new RPC call.
// Order of parameters: Return type, name, arg type
//
RPC_CALL(RemoteStopwatch, CreateStopwatch     , std::string  )
RPC_CALL(Void,            StartStopwatch   , RemoteStopwatch )
RPC_CALL(unsigned,        CheckStopwatch   , RemoteStopwatch )
RPC_CALL(unsigned,        StopStopwatch    , RemoteStopwatch )
RPC_CALL(Void,            DestroyStopwatch , RemoteStopwatch )


#endif // RPC_COMMANDS_H
