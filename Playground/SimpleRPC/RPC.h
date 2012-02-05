#ifndef RPC_H_INCLUDED
#define RPC_H_INCLUDED


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
        std::cout << "*** BEGIN " << Command::Name() << std::endl;
        std::cout << "Serialized arg: " << serialized << std::endl;
        Arg arg = deserialize<Arg>(serialized);
        Ret ret = Command::execute(*this, arg);
        std::cout << "Serialized result: " << serialize(ret) << std::endl;
        std::cout << "END " << Command::Name() << std::endl << std::endl;
        return serialize(ret);
    }

    std::string process(const NameAndArg & inNameAndArg);

    std::vector<std::string> getRegisteredCommands();

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

#endif

#ifdef RPC_CLIENT


class RPCServer;


class RPCClient
{
public:
    RPCClient(const std::string & host, short port) :
        mUDPClient(host, port)
    {

    }

    template<typename C>
    typename C::Ret send(const C & command)
    {
        std::string result = mUDPClient.send(serialize(NameAndArg(command.name(), serialize(command.arg()))));
        RetOrError retOrError = deserialize<RetOrError>(result);
        if (retOrError.get_head())
        {
            return deserialize<typename C::Ret>(retOrError.get<1>());
        }
        else
        {
            throw std::runtime_error("Server error: " + retOrError.get<1>());
        }
    }

private:
    UDPClient mUDPClient;
};

#endif


#define RPC_GENERATE_COMMAND(NAME, Signature) \
    struct NAME : public Command<Signature> { \
        typedef Command<Signature> Base; \
        typedef Base::Arg Arg; \
        typedef Base::Ret Ret; \
        static std::string Name() { return #NAME; } \
        NAME(const Arg & inArgs) : Command<Ret(Arg)>(Name(), inArgs) { } \
        static Ret execute(RPCServer & server, const Arg & arg); \
    };

#ifdef RPC_SERVER
#define RPC_REGISTER(Name, Statement) \
    namespace Registration { struct RunOnStartup_##Name { RunOnStartup_##Name() { Statement; } }; static RunOnStartup_##Name g##RunOnStartup_##Name; }
#else
#define RPC_REGISTER(Name, Statement)
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
struct Command
{
    typedef Signature_ Signature;
    typedef Command<Signature> This;
    typedef typename Decompose<Signature>::Ret Ret;
    typedef typename Decompose<Signature>::Arg Arg;

    Command(const std::string & inName, const Arg & inArg) :
        mName(inName),
        mArg(inArg)
    {
    }

    const std::string & name() const { return mName; }

    const Arg & arg() const { return mArg; }

private:
    std::string mName;
    Arg mArg;
};


/**
 * RPC_COMMAND macro for defining remote procedure calls.
 *
 * Technically the macro only takes two arguments: name and signature:
 *
 *     RPC_COMMAND(Name, Signature)
 *
 * The signature consists of a return value and one argument.
 * If you want multiple arguments you can use a container, tuple of struct.
 * You can emulate void with Void.
 *
 * Here's a simple RPC call for adding two numbers:
 *
 *     RPC_COMMAND(Add, int(tuple<int, int>))
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
 * There is also the Foreach template which enables us to apply same method on
 * a vector of objects:
 *
 *     std::vector<tuple<int, int> > args = ...;
 *     std::vector<int> sums = Foreach<Add>(args).send();
 *
 * Serialization works out-of-the box for builtin types, most standard containers,
 * boost tuple types and any combination of these. User defined structs and
 * classes must be made serializable. See the boost documentation for more info.
 */
#ifdef RPC_SERVER

#define RPC_COMMAND(Name, Signature) \
    RPC_GENERATE_COMMAND(Name, Signature) \
    RPC_REGISTER(Name, Register<Name>())

#else

#define RPC_COMMAND(Name, Signature) \
    RPC_GENERATE_COMMAND(Name, Signature)

#endif


#endif // RPC_H_INCLUDED
