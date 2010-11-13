#ifndef LOCATIONID_H_INCLUDED
#define LOCATIONID_H_INCLUDED


#include <string>


namespace HSServer {


    // Enumeration of all the http-rest resources that our application recognizes.
    enum ResourceId
    {
        ResourceId_HighScore,
        ResourceId_HighScorePostForm,
        ResourceId_HighScorePostConfirmation,
        ResourceId_HighScoreDeleteForm,
        ResourceId_HighScoreDeleteConfirmation,
        ResourceId_HallOfFame,

    };

    template<ResourceId>
    struct ResourceId2String
    {
    };

#define HSServer_LocationId2String(ResourceId, ResourcePath)        \
    template<>                                                      \
    struct ResourceId2String<ResourceId>                            \
    {                                                               \
        static const char * GetLocation() { return ResourcePath; }  \
    }

    HSServer_LocationId2String(ResourceId_HighScore,                    "hs");    
    HSServer_LocationId2String(ResourceId_HighScorePostForm,            "hs/post-form");
    HSServer_LocationId2String(ResourceId_HighScorePostConfirmation,    "hs/post-confirmation");
    HSServer_LocationId2String(ResourceId_HighScoreDeleteForm,          "hs/delete-form");
    HSServer_LocationId2String(ResourceId_HighScoreDeleteConfirmation,  "hs/delete-confirmation");    
    HSServer_LocationId2String(ResourceId_HallOfFame,                   "hof");


    template<ResourceId _ResourceId>
    class LocationPolicy
    {
    public:
        static const char * GetLocation() { return ResourceId2String<_ResourceId>::GetLocation(); }
    };


} // namespace HSServer


#endif // LOCATIONID_H_INCLUDED
