#ifndef RPC_REMOTEOBJECTS_H
#define RPC_REMOTEOBJECTS_H


#include "RPC/RemoteObject.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>


namespace RPC {


class RemoteServer : public RemoteObject
{
public:
    static const char * ClassName() { return "Server"; }

    RemoteServer() :
        RemoteObject(ClassName())
    {
    }

    RemoteServer(RemotePtr inRemotePtr, const std::string & inURL) :
        RemoteObject(ClassName(), inRemotePtr),
        mURL(inURL)
    {
    }

    const std::string & url() const { return mURL; }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        boost::serialization::base_object<RemoteObject>(*this);
        ar & mURL;
        (void)version;
    }

private:
    std::string mURL;
};


class RemoteStopwatch : public RemoteObject
{
public:
    static const char * ClassName() { return "Stopwatch"; }

    RemoteStopwatch() :
        RemoteObject(ClassName())
    {

    }

    RemoteStopwatch(RemotePtr inRemotePtr, const std::string & inName) :
        RemoteObject(ClassName(), inRemotePtr),
        mName(inName)
    {
    }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        boost::serialization::base_object<RemoteObject>(*this);
        ar & mName;
        (void)version;
    }

private:
    std::string mName;
};


} // namespace RPC


namespace boost {
namespace serialization {


using boost::tuples::tuple;


template<class Archive, class T0>
void serialize(Archive & ar, tuple<T0> & value, const unsigned int)
{
    RPC::serialize_tuple(ar, value);
}


template<class Archive, typename T0, typename T1>
void serialize(Archive & ar, tuple<T0, T1> & value, const unsigned int)
{
    RPC::serialize_tuple(ar, value);
}


template<class Archive, typename T0, typename T1, typename T2>
void serialize(Archive & ar, tuple<T0, T1, T2> & value, const unsigned int)
{
    RPC::serialize_tuple(ar, value);
}


} // namespace serialization
} // namespace boost


#endif // RPC_REMOTEOBJECTS_H
