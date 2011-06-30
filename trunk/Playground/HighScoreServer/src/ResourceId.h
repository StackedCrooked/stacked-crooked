#ifndef RESOURCEID_H_INCLUDED
#define RESOURCEID_H_INCLUDED


#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>


namespace HSServer {


typedef int ResourceId;


class Resource
{
public:
    Resource(ResourceId inResourceId, const std::string & inLocation) :
        mResourceId(inResourceId),
        mLocation(inLocation)
    {
    }

    ResourceId resourceId() const
    { return mResourceId; }

    const std::string & location() const
    { return mLocation; }

private:
    ResourceId mResourceId;
    std::string mLocation;
};


class ResourceManager
{
public:
    static ResourceManager & Instance()
    {
        static ResourceManager fInstance;
        return fInstance;
    }

    template<class T>
    void registerResource()
    {
        mResourcesById.insert(std::make_pair(T::GetId(), T::GetLocation()));
        mResourcesByLocation.insert(std::make_pair(T::GetLocation(), T::GetId()));
    }

    const std::string & getResourceLocation(ResourceId inId) const
    {
        ResourcesById::const_iterator it = mResourcesById.find(inId);
        if (it != mResourcesById.end())
        {
            return it->second;
        }
        std::stringstream ss;
        ss << "Invalid resource id: " << inId;
        throw std::invalid_argument(ss.str());
    }

    ResourceId getResourceId(const std::string & inLocation) const
    {
        ResourcesByLocation::const_iterator it = mResourcesByLocation.find(inLocation);
        if (it != mResourcesByLocation.end())
        {
            return it->second;
        }
        throw std::invalid_argument(std::string("Invalid resource location: ") + inLocation);
    }

private:
    typedef std::map<ResourceId, std::string> ResourcesById;
    ResourcesById mResourcesById;

    typedef std::map<std::string, ResourceId> ResourcesByLocation;
    ResourcesByLocation mResourcesByLocation;
};


template<class T>
class Registrator
{
public:
    Registrator()
    {
        ResourceManager::Instance().registerResource<T>();
    }
};


#define DEFINE_HTTP_RESOURCE(RESOURCE_NAME, RESOURCE_LOCATION) \
struct RESOURCE_NAME : public Resource { \
    enum { Id = __LINE__ }; \
    static ResourceId GetId() { return Id; } \
    static const char * GetLocation() { return RESOURCE_LOCATION; } \
    RESOURCE_NAME() : Resource(Id, GetLocation()) {} \
}; \
static Registrator<RESOURCE_NAME> gRegistrator_##RESOURCE_NAME;


} // namespace HSServer


#endif // RESOURCEID_H_INCLUDED
