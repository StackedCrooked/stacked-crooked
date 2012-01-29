#ifndef RPC_REMOTEOBJECT_H
#define RPC_REMOTEOBJECT_H


#include <string>


struct RemotePtr
{
    RemotePtr() : mValue(0) {}

    RemotePtr(long inValue) : mValue(inValue) {}

    template<typename T>
    RemotePtr(T * ptr) : mValue(reinterpret_cast<long>(ptr)) {}

    long value() const { return mValue; }

    template<typename T>
    T * cast() const { return reinterpret_cast<T*>(mValue); }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & mValue;
    }

    long mValue;
};


inline bool operator<(const RemotePtr & lhs, const RemotePtr & rhs)
{
    return lhs.value() < rhs.value();
}


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

private:
    std::string mClassName;
    RemotePtr mRemotePtr;
};


template<class T>
inline bool operator<(const RemoteObject<T> & lhs, const RemoteObject<T> & rhs)
{
    return lhs.remotePtr() < rhs.remotePtr();
}


#endif // RPC_REMOTEOBJECT_H
