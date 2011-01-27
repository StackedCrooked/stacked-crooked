#ifndef REQUESTMETHOD_H_INCLUDED
#define REQUESTMETHOD_H_INCLUDED


#include <string>
#include <stdexcept>


namespace HSServer {


/**
 * Method
 *
 * Enumerates the HTTP methods.
 */
enum Method
{
    Method_Nil,
    Method_Begin,
    Method_Get = Method_Begin,
    Method_Post,
    Method_Put,
    Method_Delete,
    Method_End
};


typedef const char * MethodName;


/**
 * ToString
 *
 * Converts a MethodName to a string object.
 */
inline MethodName ToString(Method inMethod)
{
    if (inMethod < Method_Begin || inMethod >= Method_End)
    {
        throw std::out_of_range("Method");
    }
    static const MethodName cMethodNames[] =
    {
        "NULL",
        "GET",
        "POST",
        "PUT",
        "DELETE"
    };
    return cMethodNames[int(inMethod)];
}


/**
 * ParseMethod
 *
 * Converts a string object back into a method enum.
 */
Method ParseMethod(const std::string & inMethod);


} // namespace HSServer


#endif // REQUESTMETHOD_H_INCLUDED
