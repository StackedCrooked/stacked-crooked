#ifndef EXCEPTION_H_INCLUDED
#define EXCEPTION_H_INCLUDED


#include <stdexcept>


namespace HSServer
{

    class MissingArgumentException : public std::runtime_error
    {
    public:
        MissingArgumentException(const std::string & inMessage);
    };


} // namespace HSServer


#endif // EXCEPTION_H_INCLUDED
