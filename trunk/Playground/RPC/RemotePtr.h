#ifndef RPC_REMOTEPTR_H
#define RPC_REMOTEPTR_H


#include <string>


template<typename T>
std::string serialize(const T & value)
{
    std::ostringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa << value;
    return ss.str();
}


template<typename T>
T deserialize(const std::string & buffer)
{
    std::istringstream ss(buffer);
    boost::archive::text_iarchive ia(ss);
    T ret;
    ia >> ret;
    return ret;
}


struct RemotePtr
{
    RemotePtr() : mValue(0) {}

    RemotePtr(long inValue) : mValue(inValue) {}

    template<typename T>
    RemotePtr(T * ptr) : mValue(reinterpret_cast<long>(ptr)) {}

    long value() const { return mValue; }

    template<typename T>
    const T & cast() const { return *reinterpret_cast<T*>(mValue); }

    template<typename T>
    T & cast() { return *reinterpret_cast<T*>(mValue); }

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
