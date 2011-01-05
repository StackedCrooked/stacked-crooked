#ifndef REQUESTMETHOD_H_INCLUDED
#define REQUESTMETHOD_H_INCLUDED


#include <string>


namespace HSServer {


enum RequestMethod
{
    RequestMethod_Begin,
    RequestMethod_Get = RequestMethod_Begin,
    RequestMethod_Post,
    RequestMethod_Put,
    RequestMethod_Delete,
    RequestMethod_End
};

	
typedef const char * RequestMethodName;
static const RequestMethodName cRequestMethodNames[] =
{
	"GET",
	"POST",
	"PUT",
	"DELETE"
};


const char * ToString(RequestMethod inRequestMethod);
RequestMethod ParseRequestMethod(const std::string & inMethod);


} // namespace HSServer


#endif // REQUESTMETHOD_H_INCLUDED
