#ifndef LOCATIONID_H_INCLUDED
#define LOCATIONID_H_INCLUDED


#include <string>


namespace HSServer {


    enum LocationId
    {
        LocationId_Unknown,
        LocationId_Begin,
        LocationId_HighScore_Get = LocationId_Begin,
        LocationId_HighScorePostForm_Get,
        LocationId_HighScore_Post,
        LocationId_HighScorePostOk_Get,
        LocationId_HighScoreDeleteForm_Get,
        LocationId_HighScore_Delete,
        LocationId_HighScoreDeleteOk_Get,
        LocationId_HallOfFame_Get,
        LocationId_End
    };

    template<LocationId>
    struct LocationId2String
    {
    };

#define HSServer_LocationId2String(LocationId, LocationString)    \
    template<>                                                    \
    struct LocationId2String<LocationId>                          \
    {                                                             \
        static const char * GetValue() { return LocationString; } \
    }

    HSServer_LocationId2String(LocationId_HighScore_Get,            "/hs");    
    HSServer_LocationId2String(LocationId_HighScore_Post,           "/hs/post");
    HSServer_LocationId2String(LocationId_HighScorePostForm_Get,    "/hs/post/form");
    HSServer_LocationId2String(LocationId_HighScorePostOk_Get,      "/hs/post/ok");    
    HSServer_LocationId2String(LocationId_HighScore_Delete,         "/hs/delete");
    HSServer_LocationId2String(LocationId_HighScoreDeleteForm_Get,  "/hs/delete/form");
    HSServer_LocationId2String(LocationId_HighScoreDeleteOk_Get,    "/hs/delete/ok");    
    HSServer_LocationId2String(LocationId_HallOfFame_Get,           "/hof");


} // namespace HSServer


#endif // LOCATIONID_H_INCLUDED
