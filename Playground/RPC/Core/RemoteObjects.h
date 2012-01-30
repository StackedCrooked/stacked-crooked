#ifndef RPC_REMOTEOBJECTS_H
#define RPC_REMOTEOBJECTS_H


#include "RemoteObject.h"
#include "Stopwatch.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <vector>


struct RemoteStopwatch : public RemoteObject<Stopwatch>
{
    typedef RemoteObject<Stopwatch> Base;

    RemoteStopwatch() {}

    RemoteStopwatch(const Stopwatch & inStopwatch) :
        Base(inStopwatch.id()),
        mName(inStopwatch.name())
    {
    }

    const std::string & name() const { return mName; }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & boost::serialization::base_object<Base>(*this);
        ar & mName;
    }

    std::string mName;
};


typedef std::vector<RemoteStopwatch> RemoteStopwatches;


#endif // RPC_REMOTEOBJECTS_H
