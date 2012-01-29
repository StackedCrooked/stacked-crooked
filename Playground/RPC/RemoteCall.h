#ifndef RPC_COMMAND_H
#define RPC_COMMAND_H


#include "Client.h"
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


#if ENABLE_TRACE
#define TRACE std::cout << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << std::endl;
#else
#define TRACE
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

    #if RPC_CLIENT
    Ret send()
    {
        if (Redirector::Empty())
        {
            throw std::runtime_error("No default destination configured.");
        }
        return Redirector::Get().send(*this);
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

#if RPC_SERVER
    typedef typename C::Arg A;
    typedef typename C::Ret R;

    static std::vector<R> execute(const std::vector<A> & arg)
    {
        TRACE
        std::vector<R> result;
        for (std::size_t idx = 0; idx < arg.size(); ++idx)
        {
            result.push_back(C::execute(A(arg[idx])));
        }
        return result;
    }
#endif

protected:
    RemoteBatchCall(const Arg & inArg) : Base(Name(), inArg) { }
};


template<typename C>
void Register();


template<typename C>
struct Batch;


//
// Helper macros for the RPC_CALL macro.
//
#define RPC_GENERATE_COMMAND(NAME, SIGNATURE) \
    struct NAME : public RemoteCall<SIGNATURE> { \
        typedef RemoteCall<SIGNATURE> Base; \
        typedef Base::Arg Arg; \
        typedef Base::Ret Ret; \
        static std::string Name() { return #NAME; } \
        NAME(const Arg & inArgs) : RemoteCall<Ret(Arg)>(Name(), inArgs) { } \
        static Ret execute(const Arg & arg); \
    };

#define RPC_GENERATE_BATCH_COMMAND(NAME, SIGNATURE) \
    template<> \
    struct Batch<NAME> : public RemoteBatchCall<NAME> { \
        typedef RemoteBatchCall<NAME>::Arg Arg; \
        typedef RemoteBatchCall<NAME>::Ret Ret; \
        Batch(const Arg & args) : RemoteBatchCall<NAME>(args) { } \
    };


//
// The server can provide implementations for the RPC calls by
// implementating the "execute" method in the generated command.
//
#if RPC_SERVER

#define RPC_REGISTER_COMMAND(NAME) \
    struct NAME##Registrator { NAME##Registrator() { Register<NAME>(); } } g##NAME##Registrator;

#define RPC_REGISTER_BATCH_COMMAND(NAME) \
    struct Batch##NAME##Registrator { \
        Batch##NAME##Registrator() { Register< Batch<NAME> >(); } \
    }; \
    static Batch##NAME##Registrator g##Batch##NAME##Registrator;

#define RPC_CALL(NAME, SIGNATURE) \
    RPC_GENERATE_COMMAND(NAME, SIGNATURE) \
    RPC_REGISTER_COMMAND(NAME) \
    RPC_GENERATE_BATCH_COMMAND(NAME, SIGNATURE) \
    RPC_REGISTER_BATCH_COMMAND(NAME)

#else

#define RPC_CALL(NAME, SIGNATURE) \
	RPC_GENERATE_COMMAND(NAME, SIGNATURE) \
	RPC_GENERATE_BATCH_COMMAND(NAME, SIGNATURE)

#endif // RPC_SERVER



/**
 * RPC_CALL macro for defining remote procedure calls.
 *
 * Technically the macro only takes two arguments: name and signature:
 *
 *     RPC_CALL(Name, Signature)
 *
 * The signature always contains a return value and exactly one argument.
 * If you wan't multiple arguments you can use a struct, tuple or vector.
 * If want to return `void` then you can `return Void();`.
 *
 * A simple `Add` function for adding two numbers could be defined as:
 *
 *     // -> Declare call with a name and signature.
 *     typedef std::pair<int, int> Pair;
 *     RPC_CALL(Add, int(Pair)
 *
 *     // -> A helper class with the given name is generated.
 *     //    You need to provide an implementation for its `execute` method:
 *     int Add::execute(const Pair & value)
 *     {
 *         return value.first + value.second;
 *     }
 *
 *     // -> You can now use it as follows:
 *     int seven = Add(make_pair(3, 4)).send();
 *
 *     // -> There is also the `Batch` utility that allows to apply same
 *     //    method on multiple values:
 *     std::vector<Pair> pairs = get_some_pairs();
 *     std::vector<int> sums = Batch<Add>(pairs).send();
 *
 * Serialization works out-of-the box for builtin types, most standard containers,
 * boost tuple types and any combination of these. User defined structs and
 * classes must be made serializable by providing a `serialize` function.
 * See the boost serialization documentation for more info.
 */
RPC_CALL(CreateStopwatch, RemoteStopwatch(std::string))
RPC_CALL(StartStopwatch, Void(RemoteStopwatch))
RPC_CALL(CheckStopwatch, unsigned(RemoteStopwatch))
RPC_CALL(StopStopwatch, unsigned(RemoteStopwatch))
RPC_CALL(DestroyStopwatch, Void(RemoteStopwatch))


#endif // RPC_COMMAND_H
