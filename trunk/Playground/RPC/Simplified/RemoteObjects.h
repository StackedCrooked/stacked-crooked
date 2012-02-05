#ifndef REMOTEOBJECTS_H
#define REMOTEOBJECTS_H


#include <string>
#include <vector>


struct RemoteObject
{
    RemoteObject(long inId = 0) : mId(inId) { }

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


struct RemoteStopwatch : public RemoteObject
{

    RemoteStopwatch() {}

    RemoteStopwatch(long inId, const std::string & inName) :
        RemoteObject(inId),
        mName(inName)
    {
    }

    const std::string & name() const { return mName; }

    std::string mName;
};


typedef std::vector<RemoteStopwatch> RemoteStopwatches;


#endif // REMOTEOBJECTS_H
