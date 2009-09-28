#ifndef STYLECONTROLLER_H_INCLUDED
#define STYLECONTROLLER_H_INCLUDED


#include <string>


namespace XULWin
{
    
    class StyleController
    {
    public:
        virtual void getStyle(std::string & outValue) = 0;
        virtual void setStyle(const std::string & inValue) = 0;
    };

} // namespace XULWin

#endif // STYLECONTROLLER_H_INCLUDED
