#ifndef HTMLELEMENTS_H_INCLUDED
#define HTMLELEMENTS_H_INCLUDED


#include <iostream>
#include <string>


namespace HTML
{
    static const size_t cIndent = 2;

    class HTMLElement
    {
    public:
        HTMLElement(const std::string & inTagName, std::ostream & outStream);

        HTMLElement(const std::string & inTagName, const std::string& inText, std::ostream & outStream);

        ~HTMLElement();

    private:
        std::string whitespace(size_t n);

        std::string mTagName;
        std::string mText;
        std::ostream & mOutStream;
        static size_t sDepth;
    };

} // HTML


#endif // HTMLELEMENTS_H_INCLUDED
