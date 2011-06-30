#include "ContentType.h"
#include <stdexcept>


namespace HSServer
{

    const char * ToString(ContentType inContentType)
    {
        switch (inContentType)
        {
            case ContentType_TextPlain:
            {
                return "text/plain";
            }
            case ContentType_TextHTML:
            {
                return "text/html";
            }
            case ContentType_ApplicationXML:
            {
                return "application/xml";
            }
            case ContentType_Unknown:
            default:
            {
                return "";
            }
        }
    }


    bool FromString(const std::string & inText, ContentType & outContentType)
    {
        for (size_t idx = ContentType_Begin; idx != ContentType_End; ++idx)
        {
            ContentType ct = static_cast<ContentType>(idx);
            if (inText == ToString(ct))
            {
                outContentType = ct;
                return true;
            }
        }
        return false;
    }

} // namespace HSServer
