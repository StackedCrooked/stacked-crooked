#ifndef RPC_REMOTEOBJECT_H
#define RPC_REMOTEOBJECT_H


#include "RPC/RemotePtr.h"
#include <string>


namespace RPC {


class RemoteObject
{
public:
    RemoteObject(const std::string & inClassName, RemotePtr inRemotePtr = RemotePtr()) :
        mClassName(inClassName),
        mRemotePtr(inRemotePtr)
    {
    }

    virtual ~RemoteObject() {}

    const std::string & className() const { return mClassName; }

    const RemotePtr & remotePtr() const { return mRemotePtr; }

    void setRemotePtr(const RemotePtr & inRemotePtr) { mRemotePtr = inRemotePtr; }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int )
    {
        ar & mClassName;
        ar & mRemotePtr;
    }

private:
    std::string mClassName;
    RemotePtr mRemotePtr;
};


inline bool operator<(const RemoteObject & lhs, const RemoteObject & rhs)
{
    return lhs.remotePtr() < rhs.remotePtr();
}


} // namespace RPC


#endif // RPC_REMOTEOBJECT_H
