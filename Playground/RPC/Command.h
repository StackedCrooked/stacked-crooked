#ifndef RPC_COMMAND_H
#define RPC_COMMAND_H


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include "TupleSupport.h"
#include <sstream>
#include <string>


struct Command
{
    Command() {}

    Command(const std::string & inClassName) :
        mClassName(inClassName)
    {
    }

    const std::string & className() const { return mClassName; }

private:
    std::string mClassName;
};


typedef boost::tuples::tuple<std::string, std::string> NameAndArg;


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


#endif // RPC_COMMAND_H
