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

private:
    std::string mName;
};


} // namespace RPC


#endif // RPC_REMOTEOBJECTS_H
