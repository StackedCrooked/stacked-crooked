#include "HTMLElements.h"


namespace HTML
{
    size_t HTMLElement::sDepth(0);


    HTMLElement::HTMLElement(const std::string & inTagName, std::ostream & outStream) :
        mTagName(inTagName),
        mOutStream(outStream)
    {
        mOutStream << whitespace(sDepth) << "<" << mTagName << ">\n";
        sDepth++;
    }


    HTMLElement::HTMLElement(const std::string & inTagName, const std::string& inText, std::ostream & outStream) :
        mTagName(inTagName),
        mText(inText),
        mOutStream(outStream)
    {
        mOutStream << "" << whitespace(sDepth) << "<" << mTagName << ">" << mText << "</" << mTagName << ">\n";
        sDepth++;
    }


    HTMLElement::~HTMLElement()
    {
        sDepth--;
        if (mText.empty())
        {
            mOutStream << whitespace(sDepth) << "</" << mTagName << ">\n";
        }
    }


    std::string HTMLElement::whitespace(size_t n)
    {
        std::string result;
        for (size_t idx = 0; idx != cIndent*n; ++idx)
        {
            result += " ";
        }
        return result;
    }


} // namespace HSServer
