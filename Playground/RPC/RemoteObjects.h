#ifndef RPC_REMOTEOBJECTS_H
#define RPC_REMOTEOBJECTS_H


#include "RemoteObject.h"
#include "Stopwatch.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>


struct RemoteStopwatch : public RemoteObject<Stopwatch>
{
    RemoteStopwatch() {}

    RemoteStopwatch(const Stopwatch & inStopwatch) :
        Super(&inStopwatch),
        mName(inStopwatch.name())
    {
    }

    const std::string & name() const { return mName; }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & boost::serialization::base_object<Super>(*this);
        ar & mName;
    }

    std::string mName;
};


#endif // RPC_REMOTEOBJECTS_H
