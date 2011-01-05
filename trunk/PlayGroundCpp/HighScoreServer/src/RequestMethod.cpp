#include "RequestMethod.h"
#include <stdexcept>


namespace HSServer {


const char * ToString(RequestMethod inRequestMethod)
{
	return cRequestMethodNames[inRequestMethod];
}


RequestMethod ParseRequestMethod(const std::string & inMethod)
{
	for (unsigned int i = RequestMethod_Begin; i < RequestMethod_End; ++i)
	{
		if (cRequestMethodNames[i] == inMethod)
		{
			return static_cast<RequestMethod>(i);
		}
	}
	throw std::runtime_error("Unsupported request method: " + inMethod);
}


} // namespace HSServer
