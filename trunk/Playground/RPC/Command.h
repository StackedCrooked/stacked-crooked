#ifndef RPC_COMMAND_H
#define RPC_COMMAND_H


#include "RemoteObjects.h"
#include "Serialization.h"
#include "TupleSupport.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <string>


struct Command
{
    Command() {}

    Command(const std::string & inClassName) :
        mClassName(inClassName)
    {
    }

    const std::string & name() const { return mClassName; }

private:
    std::string mClassName;
};


using boost::tuples::tuple;
typedef std::string Name;


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
void Register();


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


template<typename Signature_>
struct ConcreteCommand : public Command
{
    typedef Signature_ Signature;
    typedef ConcreteCommand<Signature> This;
    typedef typename Decompose<Signature>::Ret Ret;
    typedef typename Decompose<Signature>::Arg Arg;

    ConcreteCommand(const std::string & inName,
                    const Arg & inArg) :
        Command(inName),
        mArg(inArg)
    {
    }

    const Arg & arg() const { return mArg; }

    Ret run()
    {
        return Ret();
    }

protected:
    typedef ConcreteCommand<Signature> Super;

private:
    Arg mArg;
};


template<class C1, class C2,
         typename C1Arg = typename C1::Arg,
         typename C1Ret = typename C1::Ret,
         typename C2Arg = typename C2::Arg,
         typename C2Ret = typename C2::Ret>
struct ChainedCommand : public ConcreteCommand<C2Ret(C1Arg)>
{
    BOOST_STATIC_ASSERT_MSG((boost::is_same<C1Ret, C2Arg>::value), "Types don't line up correctly.");

    typedef ConcreteCommand<C2Ret(C1Arg)> Super;
    typedef typename Super::Arg Arg;
    typedef typename Super::Ret Ret;

    static const char * Name() { return "ChainedCommand"; }
    ChainedCommand(const Arg & inArg) : Super(Name(), inArg) { }
};


template<typename C,
         typename Arg_ = std::vector<typename C::Arg>,
         typename Ret_ = std::vector<typename C::Ret> >
struct ParallelCommand : public ConcreteCommand<Ret_(Arg_)>
{
    typedef Arg_ Arg;
    typedef Ret_ Ret;
    typedef ConcreteCommand<Ret(Arg)> Super;
    typedef ParallelCommand<C, Arg, Ret> This;

    static const char * Name()
    {
        return "ParallelCommand";
    }

#if TARGET_IS_RPC_SERVER
    struct Registrator
    {
        Registrator()
        {
            std::cout << "Registrator for " << typeid(This).name() << std::endl;
            Register<This>();
        }

        void ping()
        {
            std::cout << "Registrator ping: " << typeid(This).name() << std::endl;
        }
    };

    static Registrator sRegistrator;
#endif

    ParallelCommand(const Arg & inArg) :
        Super(Name(), inArg)
    {
        std::cout << "ParallelCommand constructor: " << typeid(This).name() << std::endl;
#if TARGET_IS_RPC_SERVER
        sRegistrator.ping();
#endif
    }


#if TARGET_IS_RPC_SERVER
    static Ret Implement(const Arg & arg)
    {
        Ret result;
        for (std::size_t idx = 0; idx < arg.size(); ++idx)
        {
            typename C::Ret ret = C::Implement(typename C::Arg(arg[idx]));
            result.push_back(ret);
        }
        return result;
    }

    static std::string Run(const std::string & argString)
    {
        Arg arg = deserialize<Arg>(argString);
        Ret ret = This::Implement(arg);
        return serialize(ret);
    }
#else
    static Ret Implement(const Arg & arg);
    static std::string Run(const std::string & argString);
#endif
};


#if TARGET_IS_RPC_SERVER
template<typename C, typename Arg, typename Ret>
typename ParallelCommand<C, Arg, Ret>::Registrator ParallelCommand<C, Arg, Ret>::sRegistrator;
#endif


template<typename Command>
void RegisterImpl();

template<typename Command>
void Register()
{
    RegisterImpl<Command>();
    RegisterImpl< ParallelCommand<Command> >();
}

template<typename Command>
inline void RegisterImpl()
{
    std::cout << "Register " << Command::Name() << std::endl;
    GetRunners().insert(std::make_pair(Command::Name(), boost::bind(&Command::Run, _1)));
}


#endif // RPC_COMMAND_H
