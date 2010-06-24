#ifndef REQUESTMETHOD_H_INCLUDED
#define REQUESTMETHOD_H_INCLUDED


#include <string>


namespace HSServer
{

    enum RequestMethod
    {
        RequestMethod_Unknown = -1,
        RequestMethod_Get,
        RequestMethod_Begin = RequestMethod_Get,
        RequestMethod_Post,
        RequestMethod_Put,
        RequestMethod_Delete,
        RequestMethod_End
    };

    const std::string & ToString(RequestMethod inRequestMethod);


    /**
     * Helper class to overload the FromString function.
     *
     * Experimental.
     */
    class RequestMethodString : public std::string
    {
    public:
        explicit RequestMethodString(const std::string & inValue);
    };

    RequestMethod FromString(const RequestMethodString & inValue);

} // namespace HSServer


#endif // REQUESTMETHOD_H_INCLUDED
