#ifndef HTMLELEMENTS_H_INCLUDED
#define HTMLELEMENTS_H_INCLUDED


#include <iostream>
#include <string>


namespace HTMLElements
{


#define DECLARE_HTML_ELEMENT(HTML_ELEMENT, TAGNAME) \
    class HTML_ELEMENT                                                  \
    {                                                                   \
    public:                                                             \
        HTML_ELEMENT(std::ostream & ostr);                              \
        HTML_ELEMENT(std::ostream & ostr, const std::string& );         \
        ~HTML_ELEMENT();                                                \
        static const char * TagName() { return TAGNAME; }               \
    private:                                                            \
        std::string mText;                                              \
        std::ostream & mOstr;                                           \
    };

#define IMPLEMENT_HTML_ELEMENT(HTML_ELEMENT) \
    HTML_ELEMENT::HTML_ELEMENT(std::ostream & ostr) :                                       \
        mOstr(ostr)                                                                         \
    {                                                                                       \
        mOstr << "<" << TagName() << ">";                                                   \
    }                                                                                       \
    HTML_ELEMENT::HTML_ELEMENT(std::ostream & ostr, const std::string & inText) :           \
        mText(inText),                                                                      \
        mOstr(ostr)                                                                         \
    {                                                                                       \
        mOstr << "<" << TagName() << ">" << mText << "</" << TagName() << ">";              \
    }                                                                                       \
    HTML_ELEMENT::~HTML_ELEMENT()                                                           \
    {                                                                                       \
        if (mText.empty())                                                                  \
        {                                                                                   \
            mOstr << "</" << TagName() << ">";                                              \
        }                                                                                   \
    }


    DECLARE_HTML_ELEMENT(html, "html")
    DECLARE_HTML_ELEMENT(head, "head")
    DECLARE_HTML_ELEMENT(body, "body")
    DECLARE_HTML_ELEMENT(p, "p")
    DECLARE_HTML_ELEMENT(table, "table")
    DECLARE_HTML_ELEMENT(tr, "tr")
    DECLARE_HTML_ELEMENT(td, "td")


} // HTMLElements


#endif // HTMLELEMENTS_H_INCLUDED
