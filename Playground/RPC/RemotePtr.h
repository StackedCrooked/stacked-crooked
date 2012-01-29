#ifndef RPC_REMOTEPTR_H
#define RPC_REMOTEPTR_H


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


#endif // RPC_REMOTEPTR_H
