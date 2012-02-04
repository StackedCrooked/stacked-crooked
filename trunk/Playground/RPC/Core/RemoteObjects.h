#ifndef RPC_REMOTEOBJECTS_H
#define RPC_REMOTEOBJECTS_H


#include "RemoteObject.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <vector>


struct RemoteStopwatch : public RemoteObject
{

    RemoteStopwatch() {}

    RemoteStopwatch(long inId, const std::string & inName) :
        RemoteObject(inId),
        mName(inName)
    {
    }

    const std::string & name() const { return mName; }

    std::string mName;
};


typedef std::vector<RemoteStopwatch> RemoteStopwatches;


#endif // RPC_REMOTEOBJECTS_H
