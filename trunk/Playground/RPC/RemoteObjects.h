#ifndef RPC_REMOTEOBJECTS_H
#define RPC_REMOTEOBJECTS_H


#include "RPC/RemoteObject.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>


namespace RPC {


struct RemoteServer : public RemoteObject
{
    static const char * ClassName() { return "RemoteServer"; }

    RemoteServer() {}

    RemoteServer(const RemotePtr & inRemotePtr, const std::string & inURL) :
        RemoteObject(ClassName(), inRemotePtr),
        mURL(inURL)
    {
    }

    virtual ~RemoteServer() {}

    const std::string & url() const { return mURL; }

    std::string mURL;
};


class RemoteStopwatch : public RemoteObject
{
public:
    static const char * ClassName() { return "Stopwatch"; }

    RemoteStopwatch() {}

    RemoteStopwatch(const RemotePtr & inRemotePtr, const std::string & inName) :
        RemoteObject(ClassName(), inRemotePtr),
        mName(inName)
    {
    }

    const std::string & name() const { return mName; }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & boost::serialization::base_object<RemoteObject>(*this);
        ar & mName;
    }

private:
    std::string mName;
};


} // namespace RPC


namespace boost {
namespace serialization {


using namespace RPC;


template<typename Archive>
void serialize(Archive & ar, RemotePtr & rp, const unsigned int)
{
    ar & rp.mValue;
}


template<typename Archive>
void serialize(Archive & ar, RemoteObject & ro, const unsigned int)
{
    ar & ro.mClassName & ro.mRemotePtr;
}


template<typename Archive>
void serialize(Archive & ar, RemoteServer & rs, const unsigned int)
{
    ar & boost::serialization::base_object<RemoteObject>(rs);
    ar & rs.mURL;
}


} } // namespace boost::serialization


#endif // RPC_REMOTEOBJECTS_H
