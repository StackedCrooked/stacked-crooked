#ifndef RPC_REMOTEOBJECTS_H
#define RPC_REMOTEOBJECTS_H


#include "RemoteObject.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>


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


#endif // RPC_REMOTEOBJECTS_H
