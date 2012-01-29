#ifndef RPC_REMOTEOBJECT_H
#define RPC_REMOTEOBJECT_H


#include <string>


template<class LocalType>
struct RemoteObject
{
    RemoteObject(long inId = 0) :
        mId(inId)
    {
    }

    virtual ~RemoteObject() {}

    long id() const { return mId; }

    template<typename Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & mClassName & mId;
    }

private:
    std::string mClassName;
    long mId;
};


template<class T>
inline bool operator<(const RemoteObject<T> & lhs, const RemoteObject<T> & rhs)
{
    return lhs.Id() < rhs.Id();
}


#endif // RPC_REMOTEOBJECT_H
