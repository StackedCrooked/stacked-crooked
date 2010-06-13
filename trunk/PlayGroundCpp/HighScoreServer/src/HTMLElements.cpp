#include "HTMLElements.h"


namespace HTML
{
    static size_t sDepth(0);

    std::stack<CurrentOutStream *> CurrentOutStream::sInstanceStack;


    CurrentOutStream::CurrentOutStream(std::ostream & outStream) :
        mOutStream(outStream)
    {
        sInstanceStack.push(this);
    }


    CurrentOutStream::~CurrentOutStream()
    {
        sInstanceStack.pop();
    }


    std::ostream & CurrentOutStream::CurrentlyActive()
    {
        if (sInstanceStack.empty())
        {
            throw std::runtime_error("There is no currenly active HTML output stream.");
        }
        return sInstanceStack.top()->mOutStream;
    }


    void Write(const std::string & inText)
    {
        if (inText.empty())
        {
            return;
        }
        CurrentOutStream::CurrentlyActive() << Whitespace(sDepth) << inText;
    }


    std::string Whitespace(size_t n)
    {
        std::string result;
        for (size_t idx = 0; idx != cIndent*n; ++idx)
        {
            result += " ";
        }
        return result;
    }


    HTMLElement::HTMLElement(const std::string & inTagName) :
        mTagName(inTagName),
        mOutStream(CurrentOutStream::CurrentlyActive())
    {
        mOutStream << Whitespace(sDepth) << openTag() << "\n";
        sDepth++;
    }


    HTMLElement::HTMLElement(const std::string & inTagName, const std::string& inText) :
        mTagName(inTagName),
        mText(inText),
        mOutStream(CurrentOutStream::CurrentlyActive())
    {
        mOutStream << Whitespace(sDepth) << completeNode() << "\n";
        // Note: don't increment sDepth here. It won't have children.
    }


    HTMLElement::~HTMLElement()
    {
        if (mText.empty())
        {
            sDepth--;
            mOutStream << Whitespace(sDepth) << closeTag() << "\n";            
        }
    }


    std::string HTMLElement::openTag()
    {
        return OpenTag(mTagName);
    }


    std::string HTMLElement::closeTag()
    {
        return CloseTag(mTagName);
    }


    std::string HTMLElement::selfClosingTag()
    {
        return OpenCloseTag(mTagName);
    }


    std::string HTMLElement::completeNode()
    {
        return Surround(mTagName, mText);
    }

    
    std::string HTMLElement::OpenTag(const std::string & inTagName)
    {
        return "<" + inTagName + ">";
    }


    std::string HTMLElement::CloseTag(const std::string & inTagName)
    {
        return "</" + inTagName + ">";
    }


    std::string HTMLElement::OpenCloseTag(const std::string & inTagName)
    {
        return "<" + inTagName + "/>";
    }


    std::string HTMLElement::Surround(const std::string & inTagName, const std::string & inText)
    {
        return OpenTag(inTagName) + inText + CloseTag(inTagName);
    }


} // namespace HSServer
