#include "RequestMethod.h"
#include <stdexcept>


namespace HSServer {


Method ParseMethod(const std::string & inMethod)
{
	for (unsigned int i = Method_Begin; i < Method_End; ++i)
	{
		if (ToString(static_cast<Method>(i)) == inMethod)
		{
			return static_cast<Method>(i);
		}
	}
    throw std::invalid_argument("inMethod");
}


} // namespace HSServer
