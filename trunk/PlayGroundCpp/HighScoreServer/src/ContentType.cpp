#include "ContentType.h"
#include <stdexcept>


namespace HSServer
{
    
    const std::string & ToString(ContentType inContentType)
    {
        switch (inContentType)
        {
            case ContentType_Unknown:
            {
                static const std::string sResult = "ContentType_Unknown";
                return sResult;
            }
            case ContentType_TextPlain:                
            {
                static const std::string sResult = "text/plain";
                return sResult;
            }
            case ContentType_TextHTML:
            {
                static const std::string sResult = "text/html";
                return sResult;
            }
            case ContentType_TextXML:
            {
                static const std::string sResult = "text/xml";
                return sResult;
            }
            default:
            {
                throw std::logic_error("ToString(RequestMethod): invalid enum value of type RequestMethod.");
            }
        }
    }

} // namespace HSServer
