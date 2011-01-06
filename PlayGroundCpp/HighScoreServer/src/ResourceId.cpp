#include "ResourceId.h"
#include <stdexcept>


namespace HSServer {


const char * ToString(ResourceId inResourceId)
{
	return cResourceLocations[inResourceId];
}


ResourceId ParseResourceId(const std::string & inResourceId)
{
	for (unsigned int i = ResourceId_Begin; i < ResourceId_End; ++i)
	{
		if (inResourceId == ToString(static_cast<ResourceId>(i)))
		{
			return static_cast<ResourceId>(i);
		}
	}
	throw std::logic_error("Invalid ResourceId value.");
}


} // namespace HSServer
