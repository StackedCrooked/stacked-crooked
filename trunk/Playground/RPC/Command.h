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


struct CommandBase
{
    CommandBase(const std::string & inName) :
        mName(inName)
    {
    }

    const std::string & name() const { return mName; }

    template<typename C>
    static std::string Run(const std::string & serialized)
    {
        typedef typename C::Arg Arg;
        typedef typename C::Ret Ret;
        Arg arg = deserialize<Arg>(serialized);
        Ret ret = C::Implement(arg);
        return serialize(ret);
    }

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

private:
    Arg mArg;
};


//template<typename C1,
//         typename C2,
//         typename Arg = typename C1::Arg,
//         typename Ret = typename C2::Ret,
//         typename Base = ConcreteCommand<Ret(Arg)> >
//struct ChainedCommand : public Base
//{
//    BOOST_STATIC_ASSERT_MSG((boost::is_same<typename C1::Ret, typename C2::Arg>::value), "Types don't line up correctly.");

//    static std::string Name() { return "ChainedCommand"; }

//    ChainedCommand(const Arg & inArg) : Base(Name(), inArg) { }
//};


template<typename C,
         typename Arg = std::vector<typename C::Arg>,
         typename Ret = std::vector<typename C::Ret>,
         typename Base = ConcreteCommand<Ret(Arg)> >
struct ParallelCommand : public Base
{
    typedef ParallelCommand<C, Arg, Ret> This;

    static std::string Name() { return "ParallelCommand<" + C::Name() + ">"; }

    ParallelCommand(const Arg & inArg) : Base(Name(), inArg) { }

#if TARGET_IS_RPC_SERVER
    typedef typename C::Arg A;
    typedef typename C::Ret R;

    static std::vector<R> Implement(const std::vector<A> & arg)
    {
        std::vector<R> result;
        for (std::size_t idx = 0; idx < arg.size(); ++idx)
        {
            result.push_back(C::Implement(A(arg[idx])));
        }
        return result;
    }
#endif
};


typedef boost::function<std::string(const std::string)> Runner;
typedef std::map<std::string, Runner> Runners;


inline Runners & GetRunners()
{
    static Runners fRunners;
    return fRunners;
}

template<typename C>
inline void RegisterImpl()
{
    const std::string name = C::Name();
    if (GetRunners().find(name) != GetRunners().end())
    {
        std::cout << "Already registered: " << name << std::endl;
        throw std::runtime_error("Already registered: " + name);
    }
    GetRunners().insert(std::make_pair(name, boost::bind(&CommandBase::Run<C>, _1)));
}


template<typename C>
void Register()
{
    RegisterImpl<C>();
    RegisterImpl<ParallelCommand<C> >();
}


#endif // RPC_COMMAND_H
