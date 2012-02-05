#ifndef RPC_H
#define RPC_H


#include "Networking.h"
#include "Serialization.h"
#include <map>
#include <vector>


#ifdef RPC_SERVER
class RPCServer : boost::noncopyable
{
public:
    static RPCServer & Instance()
    {
        static RPCServer fInstance;
        return fInstance;
    }

    // Start listening
    void listen(unsigned port);

    // Maps the function name to it's implementation.
    // Type erasure occurs here.
    template<typename Command>
    void registerCommand()
    {
        addHandler(Command::Name(), boost::bind(&RPCServer::process<Command>, this, _1));
    }

    template<typename Command>
    std::string process(const std::string & serialized)
    {
        typedef typename Command::Arg Arg;
        typedef typename Command::Ret Ret;
        Arg arg = deserialize<Arg>(serialized);
        Ret ret = Command::execute(*this, arg);
        return serialize(ret);
    }

    std::string processRequest(const std::string & inRequest);

private:
    RPCServer();
    ~RPCServer();

    typedef boost::function<std::string(const std::string &)> Handler;
    typedef std::map<std::string, Handler> Handlers;

    void addHandler(const std::string & inName, const Handler & inHandler);

    struct Impl;
    boost::scoped_ptr<Impl> mImpl;
};


template<typename C>
void Register()
{
    RPCServer::Instance().registerCommand<C>();
}
#else
class RPCServer;
typedef UDPClient RPCClient;
#endif


template<typename Signature>
struct Decompose;


template<typename Ret_, typename Arg_>
struct Decompose<Ret_(Arg_)>
{
    typedef Arg_ Arg;
    typedef Ret_ Ret;
};


template<typename Signature_>
struct RemoteCall
{
    typedef Signature_ Signature;
    typedef RemoteCall<Signature> This;
    typedef typename Decompose<Signature>::Ret Ret;
    typedef typename Decompose<Signature>::Arg Arg;

    RemoteCall(const std::string & inName, const Arg & inArg) :
        mName(inName),
        mArg(inArg)
    {
    }

    const std::string & name() const { return mName; }

    const Arg & arg() const { return mArg; }

    #ifdef RPC_CLIENT
    Ret send(RPCClient & client)
    {
        std::string result = client.send(serialize(NameAndArg(name(), serialize(arg()))));
        RetOrError retOrError = deserialize<RetOrError>(result);
        if (retOrError.get_head())
        {
            return deserialize<Ret>(retOrError.get<1>());
        }
        else
        {
            throw std::runtime_error("Server error: " + retOrError.get<1>());
        }
    }
    #endif // RPC_CLIENT

private:
    std::string mName;
    Arg mArg;
};


template<typename C,
         typename Arg_ = std::vector<typename C::Arg>,
         typename Ret_ = std::vector<typename C::Ret>,
         typename Base = RemoteCall<Ret_(Arg_)> >
struct RemoteBatchCall : public Base
{
    typedef Arg_ Arg;
    typedef Ret_ Ret;
    typedef RemoteBatchCall<C, Arg, Ret> This;

    static std::string Name() { return "RemoteBatchCall<" + C::Name() + ">"; }

    #ifdef RPC_SERVER
    typedef typename C::Arg A;
    typedef typename C::Ret R;

    static std::vector<R> execute(RPCServer & server, const std::vector<A> & arg)
    {
        std::vector<R> result;
        for (std::size_t idx = 0; idx < arg.size(); ++idx)
        {
            result.push_back(C::execute(server, A(arg[idx])));
        }
        return result;
    }
    #endif

protected:
    RemoteBatchCall(const Arg & inArg) : Base(Name(), inArg) { }
};


template<typename C>
struct Batch;


//
// Helper macros for the RPC_CALL macro.
//
#define RPC_GENERATE_CALL(NAME, Signature) \
    struct NAME : public RemoteCall<Signature> { \
        typedef RemoteCall<Signature> Base; \
        typedef Base::Arg Arg; \
        typedef Base::Ret Ret; \
        static std::string Name() { return #NAME; } \
        NAME(const Arg & inArgs) : RemoteCall<Ret(Arg)>(Name(), inArgs) { } \
        static Ret execute(RPCServer & server, const Arg & arg); \
    };

#define RPC_GENERATE_BATCH_CALL(NAME) \
    template<> \
    struct Batch<NAME> : public RemoteBatchCall<NAME> { \
        typedef RemoteBatchCall<NAME>::Arg Arg; \
        typedef RemoteBatchCall<NAME>::Ret Ret; \
        Batch(const Arg & args) : RemoteBatchCall<NAME>(args) { } \
    };

#define RPC_RUN_ON_STARTUP(Name, Statement) \
    namespace { struct RunOnStartup_##Name { RunOnStartup_##Name() { Statement; } } g##RunOnStartup_##Name; }



/**
 * RPC_CALL macro for defining remote procedure calls.
 *
 * Technically the macro only takes two arguments: name and signature:
 *
 *     RPC_CALL(Name, Signature)
 *
 * The signature consists of a return value and one argument.
 * If you want multiple arguments you can use a container, tuple of struct.
 * You can emulate void with Void.
 *
 * Here's a simple RPC call for adding two numbers:
 *
 *     RPC_CALL(Add, int(tuple<int, int>))
 *
 * A class called Add is generated and is missing the implementation for the
 * execute() method. We have to provide the implementation:
 *
 *     int Add::execute(RPCServer & server, const tuple<int, int> & value)
 *     {
 *         return value.first + value.second;
 *     }
 *
 * That's it. Now we can try it out:
 *
 *     int seven = Add(make_tuple(3, 4)).send();
 *
 * There is also the Batch template which enables us to apply same method on
 * a vector of objects:
 *
 *     std::vector<tuple<int, int> > args = ...;
 *     std::vector<int> sums = Batch<Add>(args).send();
 *
 * Serialization works out-of-the box for builtin types, most standard containers,
 * boost tuple types and any combination of these. User defined structs and
 * classes must be made serializable. See the boost documentation for more info.
 */
#ifdef RPC_SERVER

#define RPC_CALL(Name, Signature) \
    RPC_GENERATE_CALL(Name, Signature) \
    RPC_RUN_ON_STARTUP(Name, Register<Name>()) \
    RPC_GENERATE_BATCH_CALL(Name) \
    RPC_RUN_ON_STARTUP(Batch_##Name, Register< Batch<Name> >())

#else

#define RPC_CALL(Name, Signature) \
    RPC_GENERATE_CALL(Name, Signature) \
    RPC_GENERATE_BATCH_CALL(Name)

#endif


#endif // RPC_H
