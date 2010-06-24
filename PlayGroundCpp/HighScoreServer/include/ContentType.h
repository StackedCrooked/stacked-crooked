#ifndef CONTENTTYPE_H_INCLUDED
#define CONTENTTYPE_H_INCLUDED


#include <string>


namespace HSServer
{

    enum ContentType
    {
        ContentType_Unknown = -1,
        ContentType_TextPlain,
        ContentType_Begin = ContentType_TextPlain,
        ContentType_TextHTML,
        ContentType_TextXML,
        ContentType_End
    };

    /**
     * Returns the string representation of the requested content type.
     *
     * For example:
     *   ContentType_TextPlain => "text/plain"
     *   ContentType_TextHTML => "text/html"
     *   ...
     */
    const std::string & ToString(ContentType inRequestMethod);

} // namespace HSServer


#endif // CONTENTTYPE_H_INCLUDED
