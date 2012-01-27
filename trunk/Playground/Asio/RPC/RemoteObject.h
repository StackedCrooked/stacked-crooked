#ifndef RPC_REMOTEOBJECT_H
#define RPC_REMOTEOBJECT_H


#include "RPC/RemotePtr.h"
#include <string>


namespace RPC {


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

    std::string mClassName;
    RemotePtr mRemotePtr;
};


inline bool operator<(const RemoteObject & lhs, const RemoteObject & rhs)
{
    return lhs.remotePtr() < rhs.remotePtr();
}


} // namespace RPC


#endif // RPC_REMOTEOBJECT_H
