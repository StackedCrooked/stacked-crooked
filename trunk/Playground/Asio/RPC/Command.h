#ifndef RPC_COMMAND_H
#define RPC_COMMAND_H


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <sstream>
#include <string>


namespace RPC {


struct Command
{
    Command(const std::string & inClassName) :
        mClassName(inClassName)
    {
    }

    const std::string & className() const { return mClassName; }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & mClassName;
    }

private:
    std::string mClassName;
};


template<typename Signature>
struct Decompose;


template<typename Ret_, typename Arg_>
struct Decompose<Ret_(Arg_)>
{
    typedef Arg_ Arg;
    typedef Ret_ Ret;
};


std::string receive(const std::string & name, const std::string & buffer);


template<typename T>
std::string serialize(const T & value)
{
    std::ostringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa << value;
    return ss.str();
}


template<typename T>
T deserialize(const std::string & buffer)
{
    std::istringstream ss(buffer);
    boost::archive::text_iarchive ia(ss);
    T ret;
    ia >> ret;
    return ret;
}


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
        return deserialize<Ret>(send(className(), RPC::serialize(arg())));
    }

    typedef std::string Buffer;

    Buffer send(const std::string & name, const Buffer & buffer)
    {
        return receive(name, buffer);
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        boost::serialization::base_object<Command>(*this);
        ar & mArg;
        (void)version;
    }

private:
    Arg mArg;
};


} // namespace RPC


#endif // RPC_COMMAND_H
