#ifndef RPC_COMMAND_H
#define RPC_COMMAND_H


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/tuple/tuple.hpp>
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


template<typename Ret_, typename Arg0_, typename Arg1_>
struct Decompose<Ret_(Arg0_, Arg1_)>
{
    typedef boost::tuples::tuple<Arg0_, Arg1_> Arg;
    typedef Ret_ Ret;
};


std::string receive(const std::string & name, const std::string & buffer)
{
    (void)name;
    (void)buffer;
    throw;
}


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


using boost::tuples::get;
using boost::tuples::tuple;


template<class Tuple>
struct Helper
{
    template<typename Archive>
    static void serialize(Archive & ar, Tuple & tuple)
    {
        ar & tuple.get_head();
        Helper<typename Tuple::tail_type>::serialize(ar, tuple.get_tail());
    }
};


template<>
struct Helper<boost::tuples::null_type>
{
    template<typename Archive>
    static void serialize(Archive &, const boost::tuples::null_type &) { }
};


template<typename Archive, typename Tuple>
void serialize_tuple(Archive & ar, Tuple & tuple)
{
    Helper<Tuple>::serialize(ar, tuple);
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

protected:
    typedef ConcreteCommand<Signature> Super;

private:
    Arg mArg;
};


} // namespace RPC


#endif // RPC_COMMAND_H
