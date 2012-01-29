#ifndef RPC_COMMAND_H
#define RPC_COMMAND_H


#include "RemoteObjects.h"
#include "Serialization.h"
#include "TupleSupport.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <string>


#define TRACE std::cout << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << std::endl;


struct Void
{
    template<class Archive>
    void serialize(Archive & , const unsigned int) { }
};


typedef boost::tuples::tuple<std::string, std::string> NameAndArg;
typedef boost::tuples::tuple<bool, std::string> RetOrError;


template<typename Signature>
struct Decompose;


template<typename Ret_, typename Arg_>
struct Decompose<Ret_(Arg_)>
{
    typedef Arg_ Arg;
    typedef Ret_ Ret;
};

// Create an object on the stack in the main function of the client app.
struct Destination : boost::noncopyable
{
public:
    typedef boost::function<std::string(const std::string &)> Handler;

    static bool IsSet()
    {
        return !sDestinations.empty();
    }

    static Destination & Get()
    {
        return *sDestinations.back();
    }

    template<typename Command>
    typename Command::Ret send(const Command & command)
    {
        std::string result = mHandler(serialize(NameAndArg(command.name(), serialize(command.arg()))));
        RetOrError retOrError = deserialize<RetOrError>(result);
        if (retOrError.get_head())
        {
            return deserialize<typename Command::Ret>(retOrError.get<1>());
        }
        else
        {
            throw std::runtime_error("Server error: " + retOrError.get<1>());
        }
    }

    Destination(const Handler & inHandler) :
        mHandler(inHandler)
    {
        sDestinations.push_back(this);
    }

    ~Destination()
    {
        sDestinations.pop_back();
    }

private:
    Handler mHandler;
    static std::vector<Destination*> sDestinations;
};


struct CommandBase
{
    CommandBase(const std::string & inName) :
        mName(inName)
    {
    }

    const std::string & name() const { return mName; }

private:
    std::string mName;
};


template<typename Signature_>
struct ConcreteCommand : public CommandBase
{
    typedef Signature_ Signature;
    typedef ConcreteCommand<Signature> This;
    typedef typename Decompose<Signature>::Ret Ret;
    typedef typename Decompose<Signature>::Arg Arg;

    ConcreteCommand(const std::string & inName,
                    const Arg & inArg) :
        CommandBase(inName),
        mArg(inArg)
    {
    }

    const Arg & arg() const { return mArg; }

    Ret send()
    {
        if (!Destination::IsSet())
        {
            throw std::runtime_error("No default destination configured.");
        }

        return Destination::Get().send(*this);
    }

private:
    Arg mArg;
};


#if 0
template<typename C1,
         typename C2,
         typename Arg_ = typename C1::Arg,
         typename Ret_ = typename C2::Ret,
         typename Base = ConcreteCommand<Ret_(Arg_)> >
struct ChainedCommand : public Base
{
    BOOST_STATIC_ASSERT_MSG((boost::is_same<typename C1::Ret, typename C2::Arg>::value), "Types don't line up correctly.");

    typedef Arg_ Arg;
    typedef Ret_ Ret;

    static std::string Name() { return "ChainedCommand<" + C1::Name() + ", " + C2::Name() + ">"; }

#if TARGET_IS_RPC_SERVER
    static Ret Implement(const Arg & arg)
    {
        return C2::Implement(C1::Implement(arg));
    }
#endif

protected:
    ChainedCommand(const Arg & inArg) : Base(Name(), inArg) { }
};
#endif


template<typename C,
         typename Arg = std::vector<typename C::Arg>,
         typename Ret = std::vector<typename C::Ret>,
         typename Base = ConcreteCommand<Ret(Arg)> >
struct BatchCommand : public Base
{
    typedef BatchCommand<C, Arg, Ret> This;

    static std::string Name() { return "BatchCommand<" + C::Name() + ">"; }

#if TARGET_IS_RPC_SERVER
    typedef typename C::Arg A;
    typedef typename C::Ret R;

    static std::vector<R> Implement(const std::vector<A> & arg)
    {
        TRACE
        std::vector<R> result;
        for (std::size_t idx = 0; idx < arg.size(); ++idx)
        {
            result.push_back(C::Implement(A(arg[idx])));
        }
        return result;
    }
#endif

protected:
    BatchCommand(const Arg & inArg) : Base(Name(), inArg) { }
};


template<typename C>
void Register();


template<typename C>
struct Batch;


//
// Helper macros for the RPC_CALL macro.
//
#define RPC_GENERATE_COMMAND(RET, NAME, ARG) \
    struct NAME : public ConcreteCommand<RET(ARG)> { \
        static std::string Name() { return #NAME; } \
        NAME(const Arg & inArgs) : ConcreteCommand<RET(ARG)>(Name(), inArgs) { } \
        static RET Implement(const ARG & arg); \
    };

#define RPC_GENERATE_BATCH_COMMAND(NAME, ARG) \
    template<> \
    struct Batch<NAME> : public BatchCommand<NAME> { \
        Batch(const std::vector<ARG> & args) : BatchCommand<NAME>(args) { } \
    };


//
// The server can provide implementations for the RPC calls by
// implementating the "Implement" method in the generated command.
//
#if TARGET_IS_RPC_SERVER

#define RPC_REGISTER_COMMAND(NAME) \
    struct NAME##Registrator { NAME##Registrator() { Register<NAME>(); } } g##NAME##Registrator;

#define RPC_REGISTER_BATCH_COMMAND(NAME) \
    struct Batch##NAME##Registrator { \
        Batch##NAME##Registrator() { Register< Batch<NAME> >(); } \
    }; \
    static Batch##NAME##Registrator g##Batch##NAME##Registrator;

#define RPC_CALL(R, N, A) \
    RPC_GENERATE_COMMAND(R, N, A) \
    RPC_GENERATE_BATCH_COMMAND(N, A) \
    RPC_REGISTER_COMMAND(N) \
    RPC_REGISTER_BATCH_COMMAND(N)

#else

#define RPC_CALL(R, N, A) \
	RPC_GENERATE_COMMAND(R, N, A) \
	RPC_GENERATE_BATCH_COMMAND(N, A)


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


#endif // RPC_COMMAND_H
