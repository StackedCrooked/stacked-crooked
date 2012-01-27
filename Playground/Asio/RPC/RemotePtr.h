#ifndef RPC_REMOTEPTR_H
#define RPC_REMOTEPTR_H


#include <string>


namespace RPC {


struct RemotePtr
{
    RemotePtr() : mValue(0) {}

    RemotePtr(long inValue) : mValue(inValue) {}

    long value() const { return mValue; }

    template<typename T>
    const T & cast() const { return *reinterpret_cast<T*>(mValue); }

    template<typename T>
    T & cast() { return *reinterpret_cast<T*>(mValue); }

    long mValue;
};


inline bool operator<(const RemotePtr & lhs, const RemotePtr & rhs)
{
    return lhs.value() < rhs.value();
}


} // namespace RPC


#endif // RPC_REMOTEPTR_H
