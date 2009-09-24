#ifndef TESTER_H_INCLUDED
#define TESTER_H_INCLUDED


#include <string>


namespace XULTest
{

    class Tester
    {
    public:
        void runXULSample(const std::string & inAppName);

        void runNonXULSample();
    };


} // namespace XULTest


#endif // TESTER_H_INCLUDED
