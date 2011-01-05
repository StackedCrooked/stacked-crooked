#ifndef RESOURCEID_H_INCLUDED
#define RESOURCEID_H_INCLUDED


#include <string>


namespace HSServer {


// Enumeration of all the http-rest resources that our application recognizes.
enum ResourceId
{
    ResourceId_Begin,
    ResourceId_HighScore = ResourceId_Begin,
    ResourceId_HighScorePostForm,
    ResourceId_HighScorePostConfirmation,
    ResourceId_HighScoreDeleteForm,
    ResourceId_HighScoreDeleteConfirmation,
    ResourceId_HallOfFame,
	ResourceId_End
};


typedef const char * ResourceLocation;
static const ResourceLocation cResourceLocations[] = {
	"hs",
	"hs/post-form",
	"hs/post-confirmation",
	"hs/delete-form",
	"hs/delete-confirmation",
	"hof"
};


const char * ToString(ResourceId inResourceId);
ResourceId ParseResourceId(const std::string & inResourceId);


} // namespace HSServer


#endif // RESOURCEID_H_INCLUDED
