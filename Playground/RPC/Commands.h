#ifndef RPC_COMMANDS_H
#define RPC_COMMANDS_H


#include "Command.h"


#if TARGET_IS_RPC_SERVER

#define RPC_DECLARE_CALL(RET, NAME, ARG) \
    struct NAME : public ConcreteCommand<RET(ARG)> { \
        static std::string Name() { return #NAME; } \
        NAME(const Arg & inArgs) : ConcreteCommand<RET(ARG)>(Name(), inArgs) { } \
        static RET Implement(const ARG & arg); \
    };

#define RPC_COMMAND_REGISTER(NAME) \
    struct NAME##Registrator { NAME##Registrator() { Register<NAME>(); } } g##NAME##Registrator;

#define RPC_BATCH_CALL(NAME, ARG) \
    template<> \
    struct Batch<NAME> : public BatchCommand<NAME> { \
        Batch(const std::vector<ARG> & args) : BatchCommand<NAME>(args) { } \
    }; \
    struct Batch##NAME##Registrator { \
        Batch##NAME##Registrator() { Register< Batch<NAME> >(); } \
    }; \
    static Batch##NAME##Registrator g##Batch##NAME##Registrator;

#define RPC_CALL(R, N, A) \
    RPC_DECLARE_CALL(R, N, A) \
    RPC_COMMAND_REGISTER(N) \
    RPC_BATCH_CALL(N, A)

#else

#define RPC_BATCH_CALL(NAME, ARG) \
    template<> \
    struct Batch<NAME> : public BatchCommand<NAME> { \
        Batch(const std::vector<ARG> & args) : \
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
