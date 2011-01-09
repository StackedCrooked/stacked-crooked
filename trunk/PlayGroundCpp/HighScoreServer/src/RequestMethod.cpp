#include "RequestMethod.h"
#include <stdexcept>


namespace HSServer {


const char * ToString(RequestMethod inRequestMethod)
{
    if (inRequestMethod < RequestMethod_Begin || inRequestMethod >= RequestMethod_End)
    {
        throw std::out_of_range("RequestMethod");
    }
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
