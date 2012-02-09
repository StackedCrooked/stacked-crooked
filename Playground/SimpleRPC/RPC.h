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


template<typename C,
         typename A = typename C::Arg,
         typename R = typename C::Ret,
         typename Arg = std::vector<A>,
         typename Ret = std::vector<R>,
         typename Base = Command<Ret(Arg)> >
struct WithProgress : public Base
{
    static std::string Name()
    {
        return "WithProgress<" + C::Name() + ">";
    }

    WithProgress(const Arg & inArg) :
        Base(Name(), inArg)
    {
    }

    #ifdef RPC_SERVER
    static Ret execute(RPCServer & server, const std::vector<A> & args)
    {
        UDPSender sender("127.0.0.1", 9002);
        Ret ret;
        for (std::size_t idx = 0; idx < args.size(); ++idx)
        {
            const A & a = args[idx];
            ret.push_back(C::execute(server, a));
            sender.send(serialize(ret.back()));
        }
        return ret;
    }
    #endif
};


#define RPC_GENERATE_COMMAND(NAME, SIGNATURE) \
    struct NAME : public Command<SIGNATURE> { \
        typedef Command<SIGNATURE> Base; \
        typedef Base::Arg Arg; \
        typedef Base::Ret Ret; \
        static std::string Name() { return #NAME; } \
        NAME(const Arg & inArgs) : Command<Ret(Arg)>(Name(), inArgs) { } \
        static Ret execute(RPCServer & server, const Arg & arg); \
    };


#ifdef RPC_SERVER
#define RPC_REGISTER(NAME, Statement) \
    namespace Registration { struct RunOnStartup_##NAME { RunOnStartup_##NAME() { Statement; } }; static RunOnStartup_##NAME g##RunOnStartup_##NAME; }
#else
#define RPC_REGISTER(Name, Statement)
#endif


#ifdef RPC_SERVER
#define RPC_COMMAND(NAME, SIGNATURE) \
    RPC_GENERATE_COMMAND(NAME, SIGNATURE) \
    RPC_REGISTER(NAME, Register<NAME>()) \
    RPC_REGISTER(WithProgress##_##NAME, Register< WithProgress<NAME> >())
#else
#define RPC_COMMAND(NAME, SIGNATURE) \
    RPC_GENERATE_COMMAND(NAME, SIGNATURE)
#endif


#endif // RPC_H_INCLUDED
