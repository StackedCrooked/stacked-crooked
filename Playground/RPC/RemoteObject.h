#ifndef RPC_REMOTEOBJECT_H
#define RPC_REMOTEOBJECT_H


#include "RemotePtr.h"
#include <string>


template<class LocalType>
struct RemoteObject
{
    RemoteObject(RemotePtr inRemotePtr = RemotePtr()) :
        mRemotePtr(inRemotePtr)
    {
    }

    virtual ~RemoteObject() {}

    LocalType * get() const
    {
        return mRemotePtr.cast<LocalType>();
    }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & mClassName & mRemotePtr;
    }

    std::string mClassName;
    RemotePtr mRemotePtr;

protected:
    typedef RemoteObject<LocalType> Super;
};


template<class T>
inline bool operator<(const RemoteObject<T> & lhs, const RemoteObject<T> & rhs)
{
    return lhs.remotePtr() < rhs.remotePtr();
}


#endif // RPC_REMOTEOBJECT_H
