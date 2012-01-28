#ifndef RPC_REMOTEOBJECT_H
#define RPC_REMOTEOBJECT_H


#include "RemotePtr.h"
#include <string>


struct RemoteObject
{
    RemoteObject() :
        mClassName(100, 'a')
    {
    }

    RemoteObject(const std::string & inClassName, RemotePtr inRemotePtr) :
        mClassName(inClassName),
        mRemotePtr(inRemotePtr)
    {
    }

    virtual ~RemoteObject() {}

    const std::string & className() const { return mClassName; }

    const RemotePtr & remotePtr() const { return mRemotePtr; }

    void setRemotePtr(const RemotePtr & inRemotePtr) { mRemotePtr = inRemotePtr; }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & mClassName & mRemotePtr;
    }

    std::string mClassName;
    RemotePtr mRemotePtr;
};


inline bool operator<(const RemoteObject & lhs, const RemoteObject & rhs)
{
    return lhs.remotePtr() < rhs.remotePtr();
}


#endif // RPC_REMOTEOBJECT_H
