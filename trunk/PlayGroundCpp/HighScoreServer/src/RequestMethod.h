#ifndef REQUESTMETHOD_H_INCLUDED
#define REQUESTMETHOD_H_INCLUDED


#include <string>


namespace HSServer
{

    enum RequestMethod
    {
        RequestMethod_Unknown,
        RequestMethod_Begin,
        RequestMethod_Get = RequestMethod_Begin,
        RequestMethod_Post,
        RequestMethod_Put,
        RequestMethod_Delete,
        RequestMethod_End
    };

    const char * ToString(RequestMethod inRequestMethod);

    template<RequestMethod _RequestMethod>
    class RequestMethodPolicy
    {
    public:
        static RequestMethod GetRequestMethod() { return _RequestMethod; }
    };


} // namespace HSServer


#endif // REQUESTMETHOD_H_INCLUDED
