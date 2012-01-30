#ifndef RPC_REMOTEOBJECT_H
#define RPC_REMOTEOBJECT_H


#include <string>


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
        ar & mId;
    }

private:
    long mId;
};


inline bool operator<(const RemoteObject & lhs, const RemoteObject & rhs)
{
    return lhs.id() < rhs.id();
}


#endif // RPC_REMOTEOBJECT_H
